#ifndef TEXTURENOISE
#define TEXTURENOISE

#include "texture.h"
#include "../utility.h"

class TexNoise : public Texture {
public:
	TexNoise() {
		scale = 1.0;
		col = vec3(1, 1, 1);
	}

	TexNoise(float s, vec3 c) {
		scale = s;
		col = c;
	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		float n = noise(p * scale);
		return col * n;
	}

private:
	float noise(vec3 pos) const {
		vec3 id = pos.floor();

		float minimalDist = 1.0;

		float range = 1.0;
		for (float x = -range; x <= range; x++) {
			for (float y = -range; y <= range; y++) {
				for (float z = -range; z <= range; z++) {
					vec3 coord(x, y, z);
					vec3 rId = Utility::hash33(id + coord) * 0.5f + vec3(0.5, 0.5, 0.5);
					vec3 r = coord + rId;
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
	vec3 col;
};

#endif
