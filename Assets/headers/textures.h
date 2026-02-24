#pragma once

namespace assets {

using tex_uc = unsigned char;

// Need to free memory after use
tex_uc* loadTexture(const char* filepath, int* width, int* height, int* channels);

void freeTextureData(tex_uc* data);

} // namespace assets