#include <iostream>

#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <queue>

#include <filesystem>
namespace fs = std::filesystem;

#include <fstream>

#include "lm2.hpp"
#include "vertex.h"

#include "stb_image.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include "textures.h"


struct AssetRef {
	std::string type;
	std::string reference;
};

static bool CreateDirectoryRecursive(fs::path dirName) {
	if (!std::filesystem::create_directories(dirName)) {
		if (std::filesystem::exists(dirName)) {
			return true;
		}
		return false;
	}
	return true;
}

static const std::unordered_set<fs::path> imageExtensions{
	".png", ".jpg", ".jpeg", ".bmp"
};

static const std::unordered_set<fs::path> modelExtensions{
	".gltf", ".glb"
};

static void ConvertImageFile(fs::path source, std::ostream& stream) {
	stream << "img\n";

	std::string srcPath = source.string();
	int width, height, channels;
	stbi_uc* data = stbi_load(srcPath.c_str(), &width, &height, &channels, 4);

	assets::writeTexture(data, width, height, 4, stream);

	stream << "\n";

	stbi_image_free(data);
}

static void getAttribData(const tinygltf::Model& model, const tinygltf::Primitive& primitive, int attribIdx,
		const float*& data, int& stride, int& count) {
	const tinygltf::Accessor& accessor = model.accessors[attribIdx];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
	const tinygltf::Buffer& buffer = model.buffers[view.buffer];

	assert(!accessor.sparse.isSparse);

	const unsigned char* dataPtr =
		buffer.data.data() + view.byteOffset + accessor.byteOffset;

	stride = accessor.ByteStride(view);
	if (stride == 0) {
		stride = tinygltf::GetNumComponentsInType(accessor.type) *
			tinygltf::GetComponentSizeInBytes(accessor.componentType);
	}

	data = reinterpret_cast<const float*>(dataPtr);
	count = accessor.count;
}

static bool getAttribData1(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::string name,
	const float*& data, int& stride, int& count) {
	auto it = primitive.attributes.find(name);
	if (it == primitive.attributes.end())
		return false;

	getAttribData(model, primitive, it->second, data, stride, count);
	return true;
}

struct Transform {
	lm2::vec3 position;
	lm2::vec3 rotation;
	lm2::vec3 scale;
};

struct SceneData {
	std::unordered_map<fs::path, int> textureIndexes;
	std::unordered_map<fs::path, Transform> transforms;
};

static std::unordered_map<std::string, SceneData> scenes;

static void ConvertModelFile(fs::path source, std::ostream& stream, const std::vector<AssetRef>& inReferences, const std::string& scene) {
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;

	std::string err;
	std::string warn;

	if (source.extension() == ".glb") {
		loader.LoadBinaryFromFile(&model, &err, &warn, source.string());
	}
	else {
		loader.LoadASCIIFromFile(&model, &err, &warn, source.string());
	}
	std::map<int, int> texturesTmp;
	for (int i = 0; i < inReferences.size(); ++i) {
		const std::string& type = inReferences[i].type;
		if (type.starts_with("tex")) {
			std::string num = type.substr(3);
			int idx = scenes[scene].textureIndexes[inReferences[i].reference];
			texturesTmp[std::stoi(num)] = idx;
		}
	}

	if (texturesTmp.size() > 0) {
		stream << "textures ";
		for (auto& tex : texturesTmp) {
			stream << tex.second << " ";
		}
		stream << "end\n";
	}

	for (const tinygltf::Node& node : model.nodes) {
		if (node.mesh >= 0) {
			const tinygltf::Mesh& mesh = model.meshes[node.mesh];

			if (mesh.primitives.size() <= 0) {
				return;
			}

			const tinygltf::Primitive& primitive = mesh.primitives[0];
			
			assert(primitive.mode == TINYGLTF_MODE_TRIANGLES);

			if (node.skin == -1) {
				stream << "solid\n";
			}
			else {
				stream << "skinned\n";
			}

			// Vertices
			const float* posData = nullptr;
			const float* norData = nullptr;
			const float* uvData = nullptr;

			int posStride = 0;
			int norStride = 0;
			int uvStride = 0;

			int posVertexCount = 0;
			int norVertexCount = 0;
			int uvVertexCount = 0;

			getAttribData1(model, primitive, "POSITION", posData, posStride, posVertexCount);
			getAttribData1(model, primitive, "NORMAL", norData, norStride, norVertexCount);
			getAttribData1(model, primitive, "TEXCOORD_0", uvData, uvStride, uvVertexCount);
				
			renderer::vertex tmpVertex{};

			stream << (sizeof(renderer::vertex) * posVertexCount) << "\n";

			for (int i = 0; i < posVertexCount; ++i) {
				const float* p = reinterpret_cast<const float*>(
					reinterpret_cast<const unsigned char*>(posData) + posStride * i);

				const float* n = reinterpret_cast<const float*>(
					reinterpret_cast<const unsigned char*>(norData) + norStride * i);

				const float* uv = reinterpret_cast<const float*>(
					reinterpret_cast<const unsigned char*>(uvData) + uvStride * i);

				memcpy(&tmpVertex.pos, p, sizeof(lm2::vec3));
				memcpy(&tmpVertex.normal, n, sizeof(lm2::vec3));
				memcpy(&tmpVertex.uv, uv, sizeof(lm2::vec2));

				stream.write(reinterpret_cast<char*>(&tmpVertex), sizeof(renderer::vertex));
			}

			stream << "\n";

			// Indices
			const tinygltf::Accessor& accessorIdx = model.accessors[primitive.indices];
			const tinygltf::BufferView& bufferViewIdx = model.bufferViews[accessorIdx.bufferView];

			const tinygltf::Buffer& bufferIdx = model.buffers[bufferViewIdx.buffer];

			const char* indices = reinterpret_cast<const char*>(&bufferIdx.data[bufferViewIdx.byteOffset + accessorIdx.byteOffset]);

			stream << (accessorIdx.count * sizeof(uint32_t)) << "\n";

			switch (accessorIdx.componentType) {
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			{
				const uint8_t* buf = reinterpret_cast<const uint8_t*>(indices);
				for (size_t i = 0; i < accessorIdx.count; ++i) {
					uint32_t tmp = static_cast<uint32_t>(buf[i]);
					stream.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
				}
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			{
				const uint16_t* buf = reinterpret_cast<const uint16_t*>(indices);
				for (size_t i = 0; i < accessorIdx.count; ++i) {
					uint32_t tmp = static_cast<uint32_t>(buf[i]);
					stream.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
				}
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			{
				const uint32_t* buf = reinterpret_cast<const uint32_t*>(indices);
				for (size_t i = 0; i < accessorIdx.count; ++i) {
					uint32_t tmp = static_cast<uint32_t>(buf[i]);
					stream.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
				}
				break;
			}
			default:
				throw std::exception("Unsupported joint type");
			}
			
			stream << "\n";

			// Bones
			if (node.skin == -1) {
				continue;
			}

			const float* jointDataTmp = nullptr;
			const float* weightDataTmp = nullptr;

			int jointStride = 0;
			int weightStride = 0;

			int jointVertexCount = 0;
			int weightVertexCount = 0;

			bool hasJoints = getAttribData1(model, primitive, "JOINTS_0", jointDataTmp, jointStride, jointVertexCount);
			bool hasWeights = getAttribData1(model, primitive, "WEIGHTS_0", weightDataTmp, weightStride, weightVertexCount);

			auto it = primitive.attributes.find("JOINTS_0");
			const tinygltf::Accessor& jointAccessor = model.accessors[it->second];
			it = primitive.attributes.find("WEIGHTS_0");
			const tinygltf::Accessor& weightAccessor = model.accessors[it->second];

			assert(weightAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

			renderer::vertexSkinning tmpSkinning{};
			stream << (sizeof(tmpSkinning) * jointVertexCount) << "\n";


			for (int i = 0; i < jointVertexCount; ++i) {
				const float* w = reinterpret_cast<const float*>(
					reinterpret_cast<const unsigned char*>(weightDataTmp) + weightStride * i);

				memcpy(&tmpSkinning.weights, w, sizeof(tmpSkinning.weights));

				switch (jointAccessor.componentType) {
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				{
					const uint8_t* buf = reinterpret_cast<const uint8_t*>(jointDataTmp);
					tmpSkinning.indices = {
						static_cast<uint16_t>(buf[i * 4 + 0]),
						static_cast<uint16_t>(buf[i * 4 + 1]),
						static_cast<uint16_t>(buf[i * 4 + 2]),
						static_cast<uint16_t>(buf[i * 4 + 3]),
					};
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				{
					const uint16_t* buf = reinterpret_cast<const uint16_t*>(jointDataTmp);
					tmpSkinning.indices = {
						static_cast<uint16_t>(buf[i * 4 + 0]),
						static_cast<uint16_t>(buf[i * 4 + 1]),
						static_cast<uint16_t>(buf[i * 4 + 2]),
						static_cast<uint16_t>(buf[i * 4 + 3]),
					};
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				{
					const uint32_t* buf = reinterpret_cast<const uint32_t*>(jointDataTmp);
					tmpSkinning.indices = {
						static_cast<uint16_t>(buf[i * 4 + 0]),
						static_cast<uint16_t>(buf[i * 4 + 1]),
						static_cast<uint16_t>(buf[i * 4 + 2]),
						static_cast<uint16_t>(buf[i * 4 + 3]),
					};
					break;
				}
				default:
					throw std::exception("Unsupported joint type");
				}

				stream.write(reinterpret_cast<char*>(&tmpSkinning), sizeof(tmpSkinning));
			}

			stream << "\n";

			// Inverse bind pose
			const tinygltf::Skin& skin = model.skins[node.skin];
			
			const float* invPoseData = nullptr;

			int invPosStride = 0;

			int invPosCount = 0;

			getAttribData(model, primitive, skin.inverseBindMatrices, invPoseData, invPosStride, invPosCount);

			lm2::mat4 invMatTmp{};

			stream << sizeof(invMatTmp) * invPosCount << "\n";

			for (int i = 0; i < invPosCount; ++i) {
				const float* p = reinterpret_cast<const float*>(
					reinterpret_cast<const unsigned char*>(invPoseData) + invPosStride * i);

				memcpy(&invMatTmp, p, sizeof(invMatTmp));

				invMatTmp = lm2::transpose(invMatTmp);

				stream.write(reinterpret_cast<char*>(&invMatTmp), sizeof(invMatTmp));
			}
			
			stream << "\n";

			// Bind pose
			std::vector<lm2::mat4> poseMatrices;

			poseMatrices.reserve(skin.joints.size());

			std::queue<std::pair<int, int>> toProcess;

			std::unordered_map<int, int> childCount;

			for (size_t i = 0; i < skin.joints.size(); ++i) {
				if (!childCount.contains(skin.joints[i])) {
					childCount[skin.joints[i]] = 0;
				}
				for (int child : model.nodes[skin.joints[i]].children) {
					childCount[child]++;
				}
			}

			for (auto& count : childCount) {
				if (count.second == 0) {
					toProcess.push({ count.first, -1 });
				}
			}

			lm2::mat4 tmpMat{};

			while (toProcess.size() > 0) {
				std::pair<int, int> current = toProcess.back();
				toProcess.pop();

				const tinygltf::Node& boneNode = model.nodes[current.first];
				for (int child : boneNode.children) {
					toProcess.push({child, current.first});
				}

				poseMatrices.push_back(tmpMat);

				lm2::vec3 pos{};
				if (boneNode.translation.size() > 0) {
					pos.x = static_cast<float>(boneNode.translation[0]);
					pos.y = static_cast<float>(boneNode.translation[1]);
					pos.z = static_cast<float>(boneNode.translation[2]);
				}

				lm2::quaternion rot{};
				if (boneNode.rotation.size() > 0) {
					rot.x = static_cast<float>(boneNode.rotation[0]);
					rot.y = static_cast<float>(boneNode.rotation[1]);
					rot.z = static_cast<float>(boneNode.rotation[2]);
					rot.w = static_cast<float>(boneNode.rotation[3]);
				}

				lm2::vec3 scale{};
				if (boneNode.scale.size() > 0) {
					scale.x = static_cast<float>(boneNode.scale[0]);
					scale.y = static_cast<float>(boneNode.scale[1]);
					scale.z = static_cast<float>(boneNode.scale[2]);
				}

				tmpMat = lm2::position3D(pos);
			};

			size_t poseMatSize = sizeof(lm2::mat4) * poseMatrices.size();
			stream << poseMatSize << "\n";
			stream.write(reinterpret_cast<char*>(poseMatrices.data()), poseMatSize);
			stream << "\n";
		}
	}
}

struct FileData {
	fs::path filePath;
	std::vector<std::string> scenes;
	std::vector<AssetRef> outReferences;
	std::vector<AssetRef> inReferences;
};

static std::unordered_map<std::string, std::ofstream> packedFiles;

static std::vector<FileData> imageFiles;
static std::vector<FileData> modelFiles;

static void ClassifyFile(fs::path source, fs::path destination) {
	if (source.extension() == ".ref") {
		return;
	}

	fs::path ext = source.extension();

	fs::path refFilePath = source;
	refFilePath.replace_extension(source.extension().string() + ".ref");

	std::ifstream refFile(refFilePath);
	if (!refFile.is_open()) {
		return;
	}

	std::string word;

	std::vector<AssetRef> inRefs;
	std::vector<AssetRef> outRefs;
	std::vector<std::string> scenes;

	while (refFile.good()) {
		refFile >> word;
		if (word == "or") {
			AssetRef& ref = outRefs.emplace_back();
			refFile >> ref.type;
			getline(refFile, ref.reference);
		}
		else if (word == "ir") {
			AssetRef& ref = inRefs.emplace_back();
			refFile >> ref.type;
			getline(refFile, ref.reference);
		}
		else if (word == "s") {
			refFile >> word;
			scenes.push_back(word);
		}
	}

	if (scenes.size() == 0) {
		return;
	}

	refFile.close();


	for (std::string& scene : scenes) {
		if (!packedFiles.contains(scene)) {
			packedFiles[scene].open(destination / (scene + ".scene"), std::ios::binary);
		}
	}

	if (imageExtensions.contains(ext)) {
		imageFiles.emplace_back(source, scenes, outRefs, inRefs);
	}
	else if (modelExtensions.contains(ext)) {
		modelFiles.emplace_back(source, scenes, outRefs, inRefs);
	}
}

int main(int argc, char** argv) {
	if (argc == 2) {
		if (std::strcmp(argv[1], "--help") == 0) {
			std::cout << "Usage: \n";
			std::cout << "    " << argv[0] << " <source directory> <destination directory>\n";

			return 0;
		}
		else {
			std::cout << "unknown argument: " << argv[1] << "\n";
			return -1;
		}
	}
	if (argc < 3) {
		std::cout << "Invalid argument amount, see --help for more information\n";
		return -1;
	}

	fs::path source = argv[1];
	fs::path destination = argv[2];

	CreateDirectoryRecursive(destination);


	for (const fs::directory_entry& entry : fs::recursive_directory_iterator(source)) {
		if (entry.is_regular_file()) {
			fs::path path = fs::relative(entry.path(), source);
			path = path.remove_filename();

			ClassifyFile(entry.path(), destination);
		}
	}

	for (const FileData& data : imageFiles) {
		fs::path filename = data.filePath.filename();
		filename.replace_extension();
		for (const std::string& scene : data.scenes) {
			scenes[scene].textureIndexes[filename] = scenes[scene].textureIndexes.size() - 1;

			std::cout << "Writing: " << data.filePath << " - To scene: " << scene << "\n";
			ConvertImageFile(data.filePath, packedFiles[scene]);
		}
	}

	for (const FileData& data : modelFiles) {
		fs::path filename = data.filePath.filename();
		filename.replace_extension();
		for (const std::string& scene : data.scenes) {
			std::cout << "Writing: " << data.filePath << " - To scene: " << scene << "\n";

			ConvertModelFile(data.filePath, packedFiles[scene], data.inReferences, scene);
		}
	}

	for (auto& file : packedFiles) {
		file.second.close();
	}

	std::cout << "Source: " << source << "\n";
	std::cout << "Destination: " << destination << "\n";

	return 0;
}
