#include "textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

assets::tex_uc* assets::loadTexture(const char* filepath, int* width, int* height, int* channels) {
	// Temporary
	return stbi_load(filepath, width, height, channels, 4);
}

void assets::freeTextureData(tex_uc* data) {
	stbi_image_free(data);
}
