#ifndef PERLIN_H
#define PERLIN_H

#include "vec3.h"
#include "utility.h"

class Perlin {
public:
	Perlin();
	~Perlin();

	float noise3D(const vec3& p) const;
	float smoothNoise3D(const vec3& p) const;
	float fbm(const vec3& p, int depth) const;
private:
	static const int pointCount = 256;
	vec3* randVector;
	int* permX;
	int* permY;
	int* permZ;

	static int* perlinMakePerm() {
		int* p = new int[pointCount];
		for (int i = 0; i < Perlin::pointCount; i++) {
			p[i] = i;
		}
		permute(p, pointCount);
		return p;
	}

	static void permute(int* p, int n) {
		for (int i = n - 1; i > 0; i--) {
			int target = Utility::randomInt(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}
	
	static float trilinearInterpolate(vec3 c[2][2][2], float u, float v, float w) {
		// Look familiar?
		float uu = u * u * (3 - 2 * u);
		float vv = v * v * (3 - 2 * v);
		float ww = w * w * (3 - 2 * w);
		
		float accum = 0.0;
		
		// Basically we loop through the 3d array and accumulate the rand values across neighbouring cells
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					vec3 weightV(u - i, v - j, w - k);
					accum += (i * uu + (1 - i) * (1 - uu))
						* (j * vv + (1 - j) * (1 - vv))
						* (k * ww + (1 - k) * (1 - ww))
						* dot(c[i][j][k], weightV);
				}
			}
		}
		return accum;
	}
};

Perlin::Perlin() {
	randVector = new vec3[pointCount];
	for (int i = 0; i < pointCount; ++i) {
		randVector[i] = unitVector(vec3(Utility::randomDouble(-1, 1), Utility::randomDouble(-1, 1), Utility::randomDouble(-1, 1)));
	}
	permX = perlinMakePerm();
	permY = perlinMakePerm();
	permZ = perlinMakePerm();
}

Perlin::~Perlin() {
	delete[] randVector;
	delete[] permX;
	delete[] permY;
	delete[] permZ;
}

float Perlin::noise3D(const vec3& p) const {
	auto i = static_cast<int>(4 * p.getX()) & 255;
	auto j = static_cast<int>(4 * p.getY()) & 255;
	auto k = static_cast<int>(4 * p.getZ()) & 255;
	auto v = randVector[permX[i] ^ permY[j] ^ permZ[k]];

	return (v.getX() + v.getY() + v.getZ()) / 3;
}

float Perlin::smoothNoise3D(const vec3& p) const {
	// Same in shdtoy as fract()
	float u = p.getX() - floor(p.getX());
	float v = p.getY() - floor(p.getY());
	float w = p.getZ() - floor(p.getZ());

	// Same in shdtoy as floor()
	int i = static_cast<int>(floor(p.getX()));
	int j = static_cast<int>(floor(p.getY()));
	int k = static_cast<int>(floor(p.getZ()));

	vec3 c[2][2][2];

	// Loop through each axis (faster way of interpolating each axis manually, too long to write)
	for (int di = 0; di < 2; di++)
		for (int dj = 0; dj < 2; dj++)
			for (int dk = 0; dk < 2; dk++)
				c[di][dj][dk] = randVector[
					permX[(i + di) & 255] ^
						permY[(j + dj) & 255] ^
						permZ[(k + dk) & 255]
				];

	return trilinearInterpolate(c, u, v, w);
}

float Perlin::fbm(const vec3& p, int depth) const {
	float accum = 0.0;
	vec3 tempPos = p;
	float amplitude = 1.0;

	for (int i = 0; i < depth; i++) {
		accum += amplitude * smoothNoise3D(tempPos);

		// Decay
		amplitude *= 0.5;
		tempPos *= 2.0;
	}

	return fabs(accum);
}

#endif