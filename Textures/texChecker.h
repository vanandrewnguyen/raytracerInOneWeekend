#ifndef TEXTURECHECKER
#define TEXTURECHECKER

#include "texture.h"

class TexChecker : public Texture {
public:
	TexChecker();
	TexChecker(vec3 even, vec3 odd, float size);

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override;
public:
	float tileSize;
	vec3 evenCol;
	vec3 oddCol;
};


TexChecker::TexChecker() {}

TexChecker::TexChecker(vec3 even, vec3 odd, float size) {
	evenCol = even;
	oddCol = odd;
	tileSize = size;
}

vec3 TexChecker::getColourVal(float u, float v, const vec3& p) const {
	auto sine = sin(tileSize * p.getX()) * sin(tileSize * p.getY()) * sin(tileSize * p.getZ());
	return (sine < 0) ? evenCol : oddCol;
}

#endif