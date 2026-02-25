#include "textures.h"

#include <stdlib.h>

#include <fstream>

#include <iostream>

assets::tex_uc* assets::loadTexture(const char* filepath, int* width, int* height, int* channels) {
	std::ifstream file(filepath);

	file >> *width;
	file >> *height;
	file >> *channels;

	size_t size = static_cast<size_t>(*width) * (*height) * (*channels);

	tex_uc* data = reinterpret_cast<tex_uc*>(malloc(size));

	file.read(reinterpret_cast<char*>(data), size);

	file.close();

	return data;
}

void assets::saveTexture(tex_uc* data, int width, int height, int channels, const char* savePath) {
	std::ofstream file(savePath);

	file << width << " ";
	file << height << " ";
	file << channels << "\n";

	size_t size = static_cast<size_t>(width) * height * channels;

	file.write(reinterpret_cast<const char*>(data), size);

	file.close();
}

void assets::writeTexture(tex_uc* data, int width, int height, int channels, std::ostream& stream) {
	stream << width << " ";
	stream << height << " ";
	stream << channels << "\n";

	size_t size = static_cast<size_t>(width) * height * channels;

	stream.write(reinterpret_cast<const char*>(data), size);
}

assets::tex_uc* assets::readTexture(int* width, int* height, int* channels, std::istream& stream) {
	stream >> *width;
	stream >> *height;
	stream >> *channels;

	stream.ignore(1);

	size_t size = static_cast<size_t>(*width) * (*height) * (*channels);

	tex_uc* data = reinterpret_cast<tex_uc*>(malloc(size));

	stream.read(reinterpret_cast<char*>(data), size);

	return data;
}

void assets::freeTextureData(tex_uc* data) {
	free(data);
}
