#ifndef TEXTUREWORLEY
#define TEXTUREWORLEY

#include "texture.h"

class TexWorley : public Texture {
public:
	TexWorley() {

	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		return vec3(0, 0, 0);
	}
public:

};

#endif
