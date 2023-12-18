#ifndef TEXTUREWORLEY
#define TEXTUREWORLEY

#include "texture.h"
#include "../utility.h"

class TexWorley : public Texture {
public:
	TexWorley() {
		scale = 1.0;
		col0 = vec3(0, 0, 0);
		col1 = vec3(1, 1, 1);
	}

	TexWorley(float s, vec3 c0, vec3 c1) {
		scale = s;
		col0 = c0;
		col1 = c1;
	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		float worley = worleyNoise(p * scale);
		// float worley = Utility::hash21(vec2(p.getX(), p.getY())); <- stripes r cool
		// std::cout << "Worley noise at: " << u << "." << v << " is " << worley << std::endl;
		return Utility::colourRamp(col0, col1, worley);
	}

private:
	float worleyNoise(vec3 pos) const {
		vec3 id = pos.floor();
		vec3 fd = pos - id;

		float minimalDist = 1.0;

		float range = 1.0;
		for (float x = -range; x <= range; x++) {
			for (float y = -range; y <= range; y++) {
				for (float z = -range; z <= range; z++) {
					vec3 coord(x, y, z);
					vec3 rId = Utility::hash33(id + coord) * 0.5f + vec3(0.5, 0.5, 0.5);
					vec3 r = coord + rId - fd;
					float d = dot(r, r);

					if (d < minimalDist) {
						minimalDist = d;
					}
				}
			}
		}

		return 1.0 - minimalDist;
	}

public:
	float scale;
	vec3 col0;
	vec3 col1;
};

#endif
