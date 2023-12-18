#ifndef TEXTURE
#define TEXTURE

#include "../vec3.h"
#include "../vec2.h"

class Texture {
public:
	virtual vec3 getColourVal(float u, float v, const vec3& p) const = 0;
};

#endif