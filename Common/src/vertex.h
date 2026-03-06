#pragma once

#include "lm2.hpp"


namespace renderer {

	struct vertex {
		lm2::vec3 pos;
		lm2::vec3 normal;
		lm2::vec2 uv;
	};

	struct vertexSkinning {
		lm2::vec4 weights;
		lm2::vector4D<uint32_t> indices;
	};

	struct BoneData {
		lm2::mat4 transform;
		int parent;
	};

} // namespace renderer
