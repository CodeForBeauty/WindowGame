#include "textures.h"

#include "stb_image.h"

unsigned char* assets::loadTexture(const char* filepath, int* width, int* height, int* channels) {
	// Temporary
	return stbi_load(filepath, width, height, channels, 3);
}
