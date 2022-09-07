#ifndef TEXTUREPERLIN
#define TEXTUREPERLIN

#include "texture.h"
#include "../perlin.h"

class TexPerlin : public Texture {
public:
	TexPerlin() {
		freq = 1.0;
	}
	TexPerlin(float sc) {
		freq = sc;
	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		return vec3(1, 1, 1) * noise.smoothNoise3D(freq * p);
	}
public:
	Perlin noise;
	float freq;
};

#endif
