#ifndef TEXIMAGE
#define TEXIMAGE

#include "texture.h"
#include "../utility.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <iostream>

class TexImage : public Texture {
private:
	unsigned char* data;
	int width, height;
	int bytesPerLine;
public:
	const static int bytePerPixel = 3;

public:
	TexImage();
	TexImage(const char* filename);
	~TexImage();

	virtual vec3 getColourVal(float u, float v, const vec3& p) const;
};

TexImage::TexImage() {
	data = nullptr;
	width = 0;
	height = 0;
	bytesPerLine = 0;
}

TexImage::TexImage(const char* filename) {
	int componentsPerPixel = bytePerPixel;

	data = stbi_load(filename, &width, &height, &componentsPerPixel, componentsPerPixel);

	// Fail
	if (!data) {
		std::cout << "Failed to load image" << filename << std::endl;
		width = height = 0;
	}

	bytesPerLine = bytePerPixel * width;
}

TexImage::~TexImage() {
	delete data;
}

vec3 TexImage::getColourVal(float u, float v, const vec3& p) const {
	if (data == nullptr) {
		// If error, return bright red
		return vec3(1, 0, 0);
	}

	// Clamp given coords
	u = Utility::clamp(u, 0.0, 1.0);
	v = 1.0 - Utility::clamp(v, 0.0, 1.0);

	int i = static_cast<int>(u * width);
	int j = static_cast<int>(v * height);

	// Clamp map
	if (i >= width) {
		i = width - 1;
	}
	if (j >= height) {
		j = height - 1;
	}

	const float colourScale = 1.0 / 255.0;
	auto pixel = data + j * bytesPerLine + i * bytePerPixel;

	return vec3(colourScale * pixel[0], colourScale * pixel[1], colourScale * pixel[2]);
}

#endif