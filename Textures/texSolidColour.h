#ifndef TEXTURESOLID
#define TEXTURESOLID

#include "texture.h"

class TexSolidColour : public Texture {
public:
	TexSolidColour();
	TexSolidColour(vec3 col);
	TexSolidColour(float r, float g, float b);

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override;
private:
	vec3 colour;
};

TexSolidColour::TexSolidColour() {}

TexSolidColour::TexSolidColour(vec3 col) {
	colour = col;
}

TexSolidColour::TexSolidColour(float r, float g, float b) {
	colour = vec3(r, g, b);
}

vec3 TexSolidColour::getColourVal(float u, float v, const vec3& p) const {
	return colour;
}


#endif