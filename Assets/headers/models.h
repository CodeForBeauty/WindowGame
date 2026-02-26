#pragma once

#include "vertex.h"

#include <vector>
#include <istream>

namespace assets {

struct MaterialData {
	std::vector<int> boundTextures;
};

struct SolidMeshData {
	std::vector<renderer::vertex> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;

	lm2::vec3 position;
	lm2::vec3 rotation;
	lm2::vec3 scale;
};

struct SkinnedMeshData : SolidMeshData {
	std::vector<lm2::vec4> boneWeights;
	std::vector<lm2::vector4D<uint16_t>> boneIndices;
	std::vector<lm2::mat4> invBindPose;
	std::vector<lm2::mat4> bindPose;
};

bool loadObjModel(const char* filepath, std::vector<renderer::vertex>& vertices, std::vector<uint32_t>& indices);

void readSolidMeshFromMemory(SolidMeshData& data, std::istream& stream);

void readSkinnedMeshFromMemory(SkinnedMeshData& data, std::istream& stream);

} // namespace assets
