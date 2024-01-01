#ifndef TEXTURE
#define TEXTURE

#include "../vec3.h"
#include "../vec2.h"

class Texture {
public:
	// Returns colour value at a pixel location
	virtual vec3 getColourVal(float u, float v, const vec3& p) const = 0;

	// Returns the gradient value 0->1 at a pixel location
	virtual float getVal(float u, float v) const;
};

float Texture::getVal(float u, float v) const {
	return 0.0;
}

#endif