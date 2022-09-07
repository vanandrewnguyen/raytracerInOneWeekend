#ifndef TEXTUREPERLIN
#define TEXTUREPERLIN

#include "texture.h"
#include "../perlin.h"

class TexPerlin : public Texture {
public:
	TexPerlin() {}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		return vec3(1, 1, 1) * noise.noise3D(p);
	}
public:
	Perlin noise;
};

#endif
