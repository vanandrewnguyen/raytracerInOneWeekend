#ifndef TEXTUREPERLIN
#define TEXTUREPERLIN

#include "texture.h"
#include "../perlin.h"

class TexPerlin : public Texture {
public:
	TexPerlin() {
		freq = 1.0;
	}
	TexPerlin(float sc, int deg) {
		freq = sc;
		interval = deg;
	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		float n = noise.fbm(freq * p, interval);
		return vec3(1, 1, 1) * Utility::smoothStep(-0.1, 0.5, n);
	}
public:
	Perlin noise;
	float freq;
	int interval;
};

#endif
