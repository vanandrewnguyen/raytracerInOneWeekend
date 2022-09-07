#ifndef UTILITY
#define UTILITY

#include <cmath>
#include <limits>
#include <memory>
#include "vec3.h"
#include "ray.h"

namespace Utility {
	// Constants
	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;

	// Utility Functions
	double clamp(double x, double min, double max) {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	double randomDouble() {
		return (double)rand() / (RAND_MAX + 1.0);
	}

	double randomDouble(double min, double max) {
		return min + (max - min) * randomDouble();
	}

	int randomInt(int min, int max) {
		int randNum = rand() % (max - min + 1) + min;
		return randNum;
	}

	float smoothStep(float e1, float e2, float val) {
		val = clamp((val - e1) / (e2 - e1), 0.0, 1.0);
		// Hermite curve
		return val * val * (3 - 2 * val);
	}

	vec3 smoothStepVector(vec3 e1, vec3 e2, float val) {
		float valX = clamp((val - e1.getX()) / (e2.getX() - e1.getX()), 0.0, 1.0);
		float valY = clamp((val - e1.getY()) / (e2.getY() - e1.getY()), 0.0, 1.0);
		float valZ = clamp((val - e1.getZ()) / (e2.getZ() - e1.getZ()), 0.0, 1.0);
		valX = valX * valX * (3 - 2 * valX);
		valY = valY * valY * (3 - 2 * valY);
		valZ = valZ * valZ * (3 - 2 * valZ);
		vec3 out(valX, valY, valZ);

		// Hermite curve
		return out;
	}

	vec3 getSkyColour(const Ray& r) {
		vec3 unitDir = unitVector(r.getDirection());
		// Shift from -1->1 to 0->1
		float t = 0.5 * (unitDir.getY() + 1.0);

		// Interpolation
		vec3 whiteCol(1.0, 1.0, 1.0);
		vec3 skyCol(0.5, 0.7, 1.0);
		return (1.0 - t) * whiteCol + t * skyCol;
	}
}



#endif