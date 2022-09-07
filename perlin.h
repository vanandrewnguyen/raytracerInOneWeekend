#ifndef PERLIN
#define PERLIN

#include "vec3.h"
#include "utility.h"

class Perlin {
public:
	Perlin();
	~Perlin();

	float noise3D(const vec3& p) const;
	float smoothNoise3D(const vec3& p) const;
private:
	static const int pointCount = 256;
	float* randSeed;
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
	
	static float trilinearInterpolate(float c[2][2][2], double u, double v, double w) {
		float accum = 0.0;
		
		// Basically we loop through the 3d array and accumulate the rand values across neighbouring cells
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					accum += (i * u + (1 - i) * (1 - u)) *
					(j * v + (1 - j) * (1 - v)) *
					(k * w + (1 - k) * (1 - w)) * c[i][j][k];

		return accum;
	}
};

Perlin::Perlin() {
	randSeed = new float[pointCount];
	for (int i = 0; i < pointCount; ++i) {
		randSeed[i] = Utility::randomDouble();
	}
	permX = perlinMakePerm();
	permY = perlinMakePerm();
	permZ = perlinMakePerm();
}

Perlin::~Perlin() {
	delete[] randSeed;
	delete[] permX;
	delete[] permY;
	delete[] permZ;
}

float Perlin::noise3D(const vec3& p) const {
	int i = static_cast<int>(4 * p.getX()) & 255;
	int j = static_cast<int>(4 * p.getY()) & 255;
	int k = static_cast<int>(4 * p.getZ()) & 255;

	return randSeed[permX[i] ^ permY[j] ^ permZ[k]];
}

float Perlin::smoothNoise3D(const vec3& p) const {
	// Same in shdtoy as fract()
	float u = p.getX() - floor(p.getX());
	float v = p.getY() - floor(p.getY());
	float w = p.getZ() - floor(p.getZ());

	// Look familiar?
	u = u * u * (3 - 2 * u);
	v = v * v * (3 - 2 * v);
	w = w * w * (3 - 2 * w);

	// Same in shdtoy as floor()
	int i = static_cast<int>(floor(p.getX()));
	int j = static_cast<int>(floor(p.getY()));
	int k = static_cast<int>(floor(p.getZ()));

	float c[2][2][2];

	// Loop through each axis (faster way of interpolating each axis manually, too long to write)
	for (int di = 0; di < 2; di++)
		for (int dj = 0; dj < 2; dj++)
			for (int dk = 0; dk < 2; dk++)
				c[di][dj][dk] = randSeed[
					permX[(i + di) & 255] ^
						permY[(j + dj) & 255] ^
						permZ[(k + dk) & 255]
				];

	return trilinearInterpolate(c, u, v, w);
}

#endif