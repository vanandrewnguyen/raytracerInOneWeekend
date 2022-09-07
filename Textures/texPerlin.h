#ifndef TEXTUREPERLIN
#define TEXTUREPERLIN

#include "texture.h"
#include "../perlin.h"

class TexPerlin : public Texture {
public:
	TexPerlin() {
		freq = 1.0;
	}
	TexPerlin(float sc, int deg, vec3 col0, vec3 col1) {
		freq = sc;
		interval = deg;
		c0 = col0;
		c1 = col1;
	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		float n = noise.fbm(freq * p, interval);
		vec3 col = Utility::smoothStepVector(c0, c1, n);
		return col * Utility::smoothStep(-0.4, 0.7, n);
	}
public:
	Perlin noise;
	float freq;
	int interval;
	vec3 c0;
	vec3 c1;
};

#endif
