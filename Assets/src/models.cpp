#include "models.h"

#include "vertex.h"

#include <vector>
#include <fstream>
#include <string>
#include <sstream>


bool assets::loadModel(const char* filepath, std::vector<renderer::vertex>& vertices, std::vector<unsigned int>& indices) {
	std::ifstream file{ filepath };

	std::string line;

	std::stringstream ss;

	std::string command;

	int currPos = 0;
	int currNor = 0;
	int currUv = 0;

	vertices.clear();
	indices.clear();

	while (std::getline(file, line)) {
		if (line[0] != '#') {
			ss.str(line);
			ss.clear();

			ss >> command;
			if (command == "v") {
				lm2::vec3 pos{};
				ss >> pos.x;
				ss >> pos.y;
				ss >> pos.z;
				if (vertices.size() <= currPos) {
					vertices.push_back({ .pos = pos });
				}
				else {
					vertices[currPos].pos = pos;
				}
				currPos++;
			}
			else if (command == "vn") {
				lm2::vec3 nor{};
				ss >> nor.x;
				ss >> nor.y;
				ss >> nor.z;
				if (vertices.size() <= currNor) {
					vertices.push_back({ .normal = nor });
				}
				else {
					vertices[currNor].normal = nor;
				}
				currNor++;
			}
			else if (command == "vt") {
				lm2::vec2 uv{};
				ss >> uv.x;
				ss >> uv.y;
				if (vertices.size() <= currNor) {
					vertices.push_back({ .uv = uv });
				}
				else {
					vertices[currNor].uv = uv;
				}
				currNor++;
			}
		}
	}

	file.close();
	return true;
}
