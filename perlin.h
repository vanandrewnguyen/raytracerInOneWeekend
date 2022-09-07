#ifndef PERLIN
#define PERLIN

#include "vec3.h"
#include "utility.h"

class Perlin {
public:
	Perlin();
	~Perlin();

	float noise3D(const vec3& p) const;
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

#endif