#ifndef TEXTUREWORLEY
#define TEXTUREWORLEY

#include "texture.h"
#include "../utility.h"

class TexWorley : public Texture {
public:
	TexWorley() {
		numCells = 16.0;
		col0 = vec3(0, 0, 0);
		col1 = vec3(1, 1, 1);
	}

	TexWorley(float nC, vec3 c0, vec3 c1) {
		numCells = nC;
		col0 = c0;
		col1 = c1;
	}

	virtual vec3 getColourVal(float u, float v, const vec3& p) const override {
		vec2 uv = vec2(u, v);
		return Utility::colourRamp(col0, col1, worleyNoise(uv));
	}

private:
	vec2 getCellPoint(vec2 cell) const {
		vec2 cellBase = cell / numCells;
		float cellX = cell.getX();
		float cellY = cell.getY();
		float noiseX = Utility::hash21(vec2(cellX, cellY));
		float noiseY = Utility::hash21(vec2(cellY, cellX));
		return cellBase + vec2(0.5, 0.5) + 1.5 * vec2(noiseX, noiseY) / numCells;
	}

	float worleyNoise(vec2 coord) const {
		vec2 cell = vec2(coord.getX() * numCells, coord.getY() * numCells);
		float dist = 1.0;

		// 5x5 neighbour cell block
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				vec2 cellPoint = getCellPoint(vec2(cell.getX() + (x-2), cell.getY() + (y-2)));
				dist = std::min(dist, Utility::distanceBetween(cellPoint, coord));
			}
		}

		vec2 n = vec2(1.0 / numCells, 1.0 / numCells);
		dist /= n.length();
		dist = 1.0 - dist;
		return dist;
	}

public:
	float numCells;
	vec3 col0;
	vec3 col1;
};

#endif
