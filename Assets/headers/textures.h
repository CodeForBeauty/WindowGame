#pragma once

#include <ostream>

namespace assets {

using tex_uc = unsigned char;

// Need to free memory after use
tex_uc* loadTexture(const char* filepath, int* width, int* height, int* channels);

void saveTexture(tex_uc* data, int width, int height, int channels, const char* savePath);

void writeTexture(tex_uc* data, int width, int height, int channels, std::ostream& stream);

void freeTextureData(tex_uc* data);

} // namespace assets
