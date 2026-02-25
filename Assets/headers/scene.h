#pragma once

#include <vector>

#include <string_view>

#include "lm2.hpp"
#include "vertex.h"

#include "textures.h"


namespace assets {

struct SceneData {
	std::vector< std::pair< std::vector<renderer::vertex>, std::vector<uint16_t> > > vertexData;
	std::vector<TextureInfo> texData;

	SceneData() {};
	~SceneData();
	SceneData(const SceneData& rhs) = delete;
	SceneData& operator=(const SceneData& rhs) = delete;
	SceneData(SceneData&& rhs) noexcept;
	SceneData& operator=(SceneData&& rhs) noexcept;
};

SceneData loadSceneFromFile(std::string_view filepath);

} // namespace assets
