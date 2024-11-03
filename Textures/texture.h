#ifndef TEXTURE_H
#define TEXTURE_H

#include "../Utils/vec3.h"
#include "../Utils/vec2.h"

namespace raytrace {
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
}

#endif