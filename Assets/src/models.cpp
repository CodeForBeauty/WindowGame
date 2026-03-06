#include "models.h"

#include "vertex.h"

#include <vector>
#include <fstream>
#include <string>
#include <sstream>


bool assets::loadObjModel(const char* filepath, std::vector<renderer::vertex>& vertices, std::vector<uint32_t>& indices) {
	std::ifstream file{ filepath };

	std::string line;

	std::stringstream ss;

	std::string command;

	vertices.clear();
	indices.clear();

	std::stringstream indStream;

	std::vector<lm2::vec3> positions;
	std::vector<lm2::vec2> uvs;
	std::vector<lm2::vec3> normals;

	while (std::getline(file, line) && line[0] != 'v')
		;

	do {
		ss.str(line);
		ss.clear();

		ss >> command;
		lm2::vec3 pos{};
		ss >> pos.x;
		ss >> pos.y;
		ss >> pos.z;
		positions.push_back(pos);
	} while (std::getline(file, line) && line[0] == 'v' && line[1] == ' ');

	do {
		ss.str(line);
		ss.clear();

		ss >> command;
		lm2::vec3 normal{};
		ss >> normal.x;
		ss >> normal.y;
		ss >> normal.z;
		normals.push_back(normal);
	} while (std::getline(file, line) && line[0] == 'v' && line[1] == 'n');

	do {
		ss.str(line);
		ss.clear();

		ss >> command;
		lm2::vec2 uv{};
		ss >> uv.x;
		ss >> uv.y;
		uvs.push_back(uv);
	} while (std::getline(file, line) && line[0] == 'v' && line[1] == 't');

	vertices.resize(positions.size());

	int currentIndex = 0;

	while (std::getline(file, line) && line[0] != 'f')
		;

	do {
		ss.str(line);
		ss.clear();

		ss >> command;

		std::string idx;

		for (int i = 0; i < 3; ++i) {
			ss >> command;
			std::stringstream idSS(command);

			int idx;
			idSS >> idx;
			idSS.get();
			currentIndex = idx - 1;
			vertices[currentIndex].pos = positions[idx - 1];

			idSS >> idx;
			idSS.get();
			vertices[currentIndex].uv = uvs[idx - 1];

			idSS >> idx;
			idSS.get();
			vertices[currentIndex].normal = normals[idx - 1];

			indices.push_back(currentIndex);
		}
	} while (std::getline(file, line) && line[0] == 'f');

	file.close();
	return true;
}

void assets::readSolidMeshFromMemory(SolidMeshData& data, std::istream& stream) {
	size_t verticesSize;
	stream >> verticesSize;
	stream.ignore(1);

	data.vertices.resize(verticesSize / sizeof(renderer::vertex));

	stream.read(reinterpret_cast<char*>(data.vertices.data()), verticesSize);

	size_t indicesSize;
	stream >> indicesSize;
	stream.ignore(1);

	data.indices.resize(indicesSize / sizeof(uint32_t));

	stream.read(reinterpret_cast<char*>(data.indices.data()), indicesSize);
}

void assets::readSkinnedMeshFromMemory(SkinnedMeshData& data, std::istream& stream) {
	size_t verticesSize;
	stream >> verticesSize;
	stream.ignore(1);

	data.vertices.resize(verticesSize / sizeof(renderer::vertex));

	stream.read(reinterpret_cast<char*>(data.vertices.data()), verticesSize);

	size_t indicesSize;
	stream >> indicesSize;
	stream.ignore(1);

	data.indices.resize(indicesSize / sizeof(uint32_t));

	stream.read(reinterpret_cast<char*>(data.indices.data()), indicesSize);

	size_t skinSize;
	stream >> skinSize;
	stream.ignore(1);

	data.skinning.resize(skinSize / (sizeof(renderer::vertexSkinning)));

	stream.read(reinterpret_cast<char*>(data.skinning.data()), skinSize);

	size_t invSize;
	stream >> invSize;
	stream.ignore(1);

	data.invBindPose.resize(invSize / sizeof(lm2::mat4));

	stream.read(reinterpret_cast<char*>(data.invBindPose.data()), invSize);

	size_t bindSize;
	stream >> bindSize;
	stream.ignore(1);

	data.bindPose.resize(bindSize / sizeof(renderer::BoneData));

	stream.read(reinterpret_cast<char*>(data.bindPose.data()), bindSize);
}
