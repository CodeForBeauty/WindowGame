#pragma once

#include "lm2.hpp"


namespace renderer {

	struct vertex {
		lm2::vec3 pos;
		lm2::vec3 normal;
		lm2::vec2 uv;
	};

	struct vertexSkinning {
		lm2::vector4D<uint16_t> indices;
		lm2::vec4 weights;
	};

} // namespace renderer
