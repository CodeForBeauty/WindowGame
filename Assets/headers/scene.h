#pragma once

#include <vector>

#include <string_view>

#include "lm2.hpp"
#include "vertex.h"

#include "textures.h"
#include "models.h"


namespace assets {

struct SceneData {
	std::vector<SolidMeshData> solidMeshes;
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
