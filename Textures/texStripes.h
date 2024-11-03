#ifndef TEXTURESTRIPES_H
#define TEXTURESTRIPES_H

#include "texture.h"
#include "../Utils/utility.h"

class TexStripes : public raytrace::Texture {
public:
	TexStripes() {
		scale = 1.0;
		col0 = vec3(0, 0, 0);
		col1 = vec3(1, 1, 1);
	}

	TexStripes(float s, vec3 c0, vec3 c1) {
		scale = s;
		col0 = c0;
		col1 = c1;
	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		float s = Utility::hash21(scale * vec2(p.getX(), p.getY()));
		return Utility::colourRamp(col0, col1, s);
	}
public:
	float scale;
	vec3 col0;
	vec3 col1;
};

#endif