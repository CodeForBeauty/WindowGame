#include "scene.h"

#include "models.h"
#include "textures.h"

#include <fstream>

using namespace assets;


assets::SceneData::~SceneData() {
	for (TextureInfo tex : texData) {
		freeTextureData(tex.pixels);
	}
}

assets::SceneData::SceneData(SceneData&& rhs) noexcept {
	std::swap(solidMeshes, rhs.solidMeshes);
	std::swap(texData, rhs.texData);
}

SceneData& assets::SceneData::operator=(SceneData&& rhs) noexcept {
	std::swap(solidMeshes, rhs.solidMeshes);
	std::swap(texData, rhs.texData);
	return *this;
}

SceneData assets::loadSceneFromFile(std::string_view filepath) {
	std::ifstream file(filepath.data(), std::ios::binary);

	if (!file.is_open()) {
		return {};
	}

	SceneData scene{};

	std::string word;

	while (!file.eof()) {
		file >> word;
		if (file.eof()) {
			break;
		}

		if (word == "img") {
			TextureInfo& tex = scene.texData.emplace_back();
			tex.pixels = readTexture(&tex.width, &tex.height, &tex.channels, file);
		}
		else if (word == "textures") {
			std::string num;
			while (num != "end") {
				file >> num;
			}
		}
		else if (word == "solid") {
			auto& mesh = scene.solidMeshes.emplace_back();
			readSolidMeshFromMemory(mesh, file);
		}
		else if (word == "skinned") {
			auto& mesh = scene.skinnedMeshes.emplace_back();
			readSkinnedMeshFromMemory(mesh, file);
		}
	}

	file.close();

	return scene;
}
