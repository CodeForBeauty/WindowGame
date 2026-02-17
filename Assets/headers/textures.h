#pragma once

namespace assets {

// Need to free memory after use
unsigned char* loadTexture(const char* filepath, int* width, int* height, int* channels);

} // namespace assets