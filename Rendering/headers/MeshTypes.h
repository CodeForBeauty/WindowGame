#pragma once

#include "lm2.hpp"

#include <stdint.h>


namespace renderer {

// TODO: Rework rotation using quaternions
struct MeshData {
	lm2::vec3 position;
	lm2::vec3 rotation;
	lm2::vec3 scale;
};

struct SolidMesh {
	uint32_t vertexOffset;
	uint32_t vertexCount;
	uint32_t indexOffset;
	uint32_t indexCount;

	MeshData data;
};

struct SkinnedMesh : SolidMesh {
	uint32_t boneIndexOffset;
	uint32_t boneIndexCount;
};

} // namespace renderer