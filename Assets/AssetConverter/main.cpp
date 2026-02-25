#include <iostream>

#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <vector>

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

	assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
	assert(!accessor.sparse.isSparse);

	const unsigned char* dataPtr =
		buffer.data.data() + view.byteOffset + accessor.byteOffset;

	stride = accessor.ByteStride(view);
	if (stride == 0)
		stride = tinygltf::GetNumComponentsInType(accessor.type) *
		tinygltf::GetComponentSizeInBytes(accessor.componentType);

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

static std::unordered_map<std::string, std::unordered_map<fs::path, int> > textureIndexes;

static void ConvertModelFile(fs::path source, std::ostream& stream, const std::vector<std::string>& inReferences, const std::string& scene) {
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

	for (int i = 0; i < inReferences.size(); ++i) {
		stream << textureIndexes[scene][inReferences[i]] << "\n";
	}

	for (const tinygltf::Node& node : model.nodes) {
		if (node.mesh >= 0) {
			const tinygltf::Mesh& mesh = model.meshes[node.mesh];
			for (const tinygltf::Primitive& primitive : mesh.primitives) {
				assert(primitive.mode == TINYGLTF_MODE_TRIANGLES);

				stream << "model\n";

				const float* posData = nullptr;
				const float* norData = nullptr;
				const float* uvData = nullptr;

				int posStride = 0;
				int norStride = 0;
				int uvStride = 0;

				int vertexCount = 0;

				getAttribData1(model, primitive, "POSITION", posData, posStride, vertexCount);
				getAttribData1(model, primitive, "NORMAL", norData, norStride, vertexCount);
				getAttribData1(model, primitive, "TEXCOORD_0", uvData, uvStride, vertexCount);
				
				renderer::vertex tmpVertex{};

				stream << sizeof(renderer::vertex) * vertexCount << "\n";

				for (int i = 0; i < vertexCount; ++i) {
					const float* p = reinterpret_cast<const float*>(
						reinterpret_cast<const unsigned char*>(posData) + posStride * i);

					const float* n = reinterpret_cast<const float*>(
						reinterpret_cast<const unsigned char*>(norData) + norStride * i);

					const float* uv = reinterpret_cast<const float*>(
						reinterpret_cast<const unsigned char*>(uvData) + uvStride * i);

					memcpy(&tmpVertex.pos, p, sizeof(float) * 3);
					memcpy(&tmpVertex.normal, n, sizeof(float) * 3);
					memcpy(&tmpVertex.uv, uv, sizeof(float) * 2);

					stream.write(reinterpret_cast<char*>(&tmpVertex), sizeof(renderer::vertex));
				}

				const tinygltf::Accessor& accessorIdx = model.accessors[primitive.indices];
				const tinygltf::BufferView& bufferViewIdx = model.bufferViews[accessorIdx.bufferView];

				const tinygltf::Buffer& bufferIdx = model.buffers[bufferViewIdx.buffer];

				const char* indices = reinterpret_cast<const char*>(&bufferIdx.data[bufferViewIdx.byteOffset + accessorIdx.byteOffset]);

				size_t indicesSize = 0;

				stream << accessorIdx.count * sizeof(unsigned int) << "\n";

				switch (accessorIdx.componentType) {
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				{
					const uint8_t* buf = reinterpret_cast<const uint8_t*>(indices);
					for (size_t i = 0; i < accessorIdx.count; ++i) {
						uint16_t tmp = buf[i];
						stream.write(reinterpret_cast<const char*>(&tmp), sizeof(uint16_t));
					}
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				{
					const uint16_t* buf = reinterpret_cast<const uint16_t*>(indices);
					for (size_t i = 0; i < accessorIdx.count; ++i) {
						uint16_t tmp = buf[i];
						stream.write(reinterpret_cast<const char*>(&tmp), sizeof(uint16_t));
					}
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				{
					const uint32_t* buf = reinterpret_cast<const uint32_t*>(indices);
					for (size_t i = 0; i < accessorIdx.count; ++i) {
						uint16_t tmp = static_cast<uint16_t>(buf[i]);
						stream.write(reinterpret_cast<const char*>(&tmp), sizeof(uint16_t));
					}
					break;
				}
				}
				
				stream << "\n";
			}
		}
	}
}

struct FileData {
	fs::path filePath;
	std::vector<std::string> scenes;
	std::vector<std::string> outReferences;
	std::vector<std::string> inReferences;
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

	std::string ref;

	std::vector<std::string> inRefs;
	std::vector<std::string> outRefs;
	std::vector<std::string> scenes;

	while (refFile.good()) {
		refFile >> ref;
		if (ref == "o") {
			refFile >> ref;
			outRefs.push_back(ref);
		}
		else if (ref == "i") {
			refFile >> ref;
			inRefs.push_back(ref);
		}
		else if (ref == "s") {
			refFile >> ref;
			scenes.push_back(ref);
		}
	}

	if (scenes.size() == 0) {
		return;
	}

	refFile.close();


	for (std::string& scene : scenes) {
		if (!packedFiles.contains(scene)) {
			packedFiles[scene].open(destination / (scene + ".scene"));
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
			textureIndexes[scene][filename] = textureIndexes[scene].size();

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
