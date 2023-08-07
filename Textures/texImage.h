#ifndef TEXIMAGE
#define TEXIMAGE

#include "texture.h"
#include "../utility.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <iostream>

class TexImage : public Texture {
private:
	const int bytesPerPixel = 3;
	unsigned char* data;
	int width, height;
	int bytesPerLine;

public:
	TexImage();
	TexImage(const char* name);
	~TexImage();

	const unsigned char* getPixelData(int x, int y) const;
	bool loadFile(const std::string filename);
	int getWidth() const;
	int getHeight() const;

	virtual vec3 getColourVal(float u, float v, const vec3& p) const;
};

TexImage::TexImage() {
	data = nullptr;
	width = 0;
	height = 0;
	bytesPerLine = 0;
}

TexImage::TexImage(const char* name) {
	std::string filename = std::string(name);

	// Hunt for the image file in some likely locations.
	if (loadFile(filename)) return;
	if (loadFile("Assets/" + filename)) return;
	if (loadFile("../Assets/" + filename)) return;
	if (loadFile("../../Assets/" + filename)) return;
	if (loadFile("../../../Assets/" + filename)) return;
	if (loadFile("../../../../Assets/" + filename)) return;
	if (loadFile("../../../../../Assets/" + filename)) return;
	if (loadFile("../../../../../../Assets/" + filename)) return;

	std::cout << "ERROR: Could not load image file '" << filename << "'.\n";
}

bool TexImage::loadFile(const std::string filename) {
	// n is a dummy variable
	auto n = bytesPerPixel; 
	data = stbi_load(filename.c_str(), &width, &height, &n, bytesPerPixel);
	bytesPerLine = width * bytesPerPixel;

	if (data != nullptr) {
		std::cout << "Successfully loaded: " << filename << std::endl;
	}

	return data != nullptr;
}

const unsigned char* TexImage::getPixelData(int x, int y) const {
	// Return the address of the three bytes of the pixel at x, y
	static unsigned char debug_col[] = { 255, 255, 255 };
	if (data == nullptr) {
		return debug_col;
	}

	x = Utility::clamp(x, 0, width);
	y = Utility::clamp(y, 0, height);

	return data + y * bytesPerLine + x * bytesPerPixel;
}

TexImage::~TexImage() {
	STBI_FREE(data);
}

vec3 TexImage::getColourVal(float u, float v, const vec3& p) const {
	if (data == nullptr || height == 0) {
		// If error, return bright red
		return vec3(1, 0, 0);
	}

	// Clamp given coords
	u = Utility::clamp(u, 0.0, 1.0);
	v = 1.0 - Utility::clamp(v, 0.0, 1.0);

	int i = static_cast<int>(u * width);
	int j = static_cast<int>(v * height);

	const float colourScale = 1.0 / 255.0;
	const unsigned char* pixel = getPixelData(i, j);

	return vec3(colourScale * pixel[0], colourScale * pixel[1], colourScale * pixel[2]);
}

int TexImage::getWidth() const { 
	return (data == nullptr) ? 0 : width; 
}

int TexImage::getHeight() const {
	return (data == nullptr) ? 0 : height; 
}

#endif