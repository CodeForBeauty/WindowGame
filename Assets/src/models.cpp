#include "models.h"

#include "vertex.h"

#include <vector>
#include <fstream>
#include <string>
#include <sstream>


bool assets::loadObjModel(const char* filepath, std::vector<renderer::vertex>& vertices, std::vector<uint16_t>& indices) {
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

void assets::readModelFromMemory(std::vector<renderer::vertex>& vertices, std::vector<uint16_t>& indices, std::istream& stream) {
	size_t verticesSize;
	stream >> verticesSize;
	stream.ignore(1);

	vertices.resize(verticesSize / sizeof(renderer::vertex));

	stream.read(reinterpret_cast<char*>(vertices.data()), verticesSize);

	size_t indicesSize;
	stream >> indicesSize;
	stream.ignore(1);

	indices.resize(indicesSize / sizeof(uint16_t));

	stream.read(reinterpret_cast<char*>(indices.data()), indicesSize);
}
