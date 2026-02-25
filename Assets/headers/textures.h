#pragma once

#include <ostream>
#include <istream>

namespace assets {

using tex_uc = unsigned char;

struct TextureInfo {
	int width;
	int height;
	int channels;
	tex_uc* pixels;
};

// Need to free memory after use
tex_uc* loadTexture(const char* filepath, int* width, int* height, int* channels);

void saveTexture(tex_uc* data, int width, int height, int channels, const char* savePath);

void writeTexture(tex_uc* data, int width, int height, int channels, std::ostream& stream);

// Needs to be freed
tex_uc* readTexture(int* width, int* height, int* channels, std::istream& stream);

void freeTextureData(tex_uc* data);

} // namespace assets
