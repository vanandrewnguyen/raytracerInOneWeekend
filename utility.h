#ifndef UTILITY
#define UTILITY

#include <cmath>
#include <limits>
#include <memory>
#include "vec3.h"
#include "vec2.h"
#include "ray.h"

namespace Utility {
	// Constants
	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;

	// Math
	double clamp(double x, double min, double max) {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	int clamp(int x, int min, int max) {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	bool isConvertibleToInt(const std::string& str) {
		try {
			std::stoi(str);
			return true;
		}
		catch (...) {
			// Failed
		}
		return false;
	}

	float distanceBetween(vec2 p1, vec2 p2) {
		float dx = p2.getX() - p1.getX();
		float dy = p2.getY() - p1.getY();
		return std::sqrt(dx * dx + dy * dy);
	}

	// Conversion
	float degToRad(float deg) {
		return deg * (pi / 180);
	}

	float radToDeg(float rad) {
		return rad * (180 / pi);
	}

	// Random generators and hashes
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

	float hash21(vec2 co) {
		return std::fmod(std::sin(co.getX() * 12.9898 + co.getY() * 78.233) * 43758.5453, 1.0);
	}

	vec3 hash33(vec3 p3) {
		vec3 p = p3 * vec3(0.1031, 0.11369, 0.13787);
		p = p.fract();
		float dotProd = dot(p, vec3(p.getY(), p.getX(), p.getZ()) + vec3(19.19, 19.19, 19.19));
		p += vec3(dotProd, dotProd, dotProd);
		return vec3(-1.0, -1.0, -1.0) + 2.0 * vec3(
			std::fmod((p.getX() + p.getY()) * p.getZ(), 1.0),
			std::fmod((p.getX() + p.getZ()) * p.getY(), 1.0),
			std::fmod((p.getY() + p.getZ()) * p.getX(), 1.0)
		);

		/*
		vec3 p3 = p * vec3(1.1031f, 1.1030f, 1.0973f);
		p3 = p3 - vec3(floor(p3.getX()), floor(p3.getY()), floor(p3.getZ()));
		p3 = p3 + vec3(19.19f, 19.19f, 19.19f);
		p3 = p3 - vec3(floor(p3.getX()), floor(p3.getY()), floor(p3.getZ()));
		return vec3(std::fmod((p3.getX() + p3.getY()) * p3.getZ(), 1.0f), std::fmod((p3.getY() + p3.getZ()) * p3.getX(), 1.0f), std::fmod((p3.getZ() + p3.getX()) * p3.getY(), 1.0f));
		*/
	}

	// Step
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

	// Rendering Colours
	vec3 colourRamp(vec3 c0, vec3 c1, float ratio) {
		ratio = std::fmax(0.0f, std::fmin(1.0f, ratio));
		return c0 + ratio * (c1 - c0);
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