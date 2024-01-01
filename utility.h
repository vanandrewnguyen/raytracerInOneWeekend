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

	float linearInterp(const float& x0, const float& y0, const float& x1, const float& y1, const float& x) {
		double output;
		if ((x1 - x0) == 0.0) {
			output = y0;
		} else {
			output = y0 + ((x - x0) * ((y1 - y0) / (x1 - x0)));
		}
		return output;
	}

	float bilinearInterp(const float& x0, const float& y0, const float& v0,
						const float& x1, const float& y1, const float& v1,
						const float& x2, const float& y2, const float& v2,
						const float& x3, const float& y3, const float& v3,
						const float& x, const float& y) {
		float p1 = linearInterp(x0, v0, x1, v1, x);
		float p2 = linearInterp(x2, v2, x3, v3, x);
		float p3 = linearInterp(y0, p1, y2, p2, y);
		return p3;
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

	float hash31(vec3 p) {
		vec3 fract = vec3(std::fmod(p.getX(), 1.0f), std::fmod(p.getY(), 1.0f), std::fmod(p.getZ(), 1.0f));
		fract = fract * vec3(.1031, .11369, .13787);
		float res = std::fmod(dot(fract, vec3(1.0f, 1.0f, 1.0f)), 19.19f);
		fract += vec3(res, res, res);

		float output = -1.0f + 2.0f * std::fmod((fract.getX() + fract.getY()) * fract.getZ(), 1.0f);

		return output;
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

	vec3 getSimpleSkyGradient(const Ray& r) {
		vec3 unitDir = unitVector(r.getDirection());
		// Shift from -1->1 to 0->1
		float t = 0.5 * (unitDir.getY() + 1.0);

		// Interpolation
		vec3 whiteCol(1.0, 1.0, 1.0);
		vec3 skyCol(0.5, 0.7, 1.0);
		return (1.0 - t) * whiteCol + t * skyCol;
	}

	vec3 getNightSkyGradient(const Ray& r) {
		vec3 unitDir = unitVector(r.getDirection());
		float t = 0.5 * (unitDir.getY() + 1.0);

		// Interpolation
		vec3 bottomCol(0,0,0);
		vec3 topCol(0.08, 0.125, 0.173);
		vec3 rampCol = (1.0 - t) * bottomCol + t * topCol;

		return rampCol;
	}

	vec3 getFakeScatteringSkyGradient(const Ray& r) {
		// Source: https://www.shadertoy.com/view/lt2SR1
		vec3 rd = unitVector(r.getDirection());
		vec3 sunDir = vec3(-2, -0.4, 0) / (vec3(-2, -0.4, 0).length());
		
		float yd = std::min(rd.getY(), 0.0f);
		rd.setY(std::max(rd.getY(), 0.0f));

		vec3 col(0, 0, 0);

		col += vec3(0.4f, 0.4f - std::exp(-rd.getY() * 20.0f) * 0.15f, 0.0f) * std::exp(-rd.getY() * 9.0f); // Red / Green 
		col += vec3(0.3f, 0.5f, 0.6f) * (1.0f - std::exp(-rd.getY() * 8.0f)) * std::exp(-rd.getY() * 0.9f); // Blue

		col = Utility::colourRamp(col * 1.2f, vec3(0.3,  0.3, 0.3), 1.0f - std::exp(yd * 100.0f)); // Fog

		col += vec3(1.0f, 0.8f, 0.55f) * pow(std::max(dot(rd, sunDir), 0.0f), 15.0f) * 0.6f; // Sun
		float flare = pow(std::max(dot(rd, sunDir), 0.0f), 150.0f) * 0.15f;
		col += vec3(flare, flare, flare);

		return col;
	}

	// Time
	void printTimeTaken(std::chrono::high_resolution_clock::time_point startTime, std::chrono::high_resolution_clock::time_point endTime) {
		std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(endTime - startTime);
		std::chrono::minutes minutes = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime) % std::chrono::hours(1);
		std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime) % std::chrono::minutes(1);

		// Print the duration
		std::cout << "Time taken to render: "
			<< hours.count() << " hours, "
			<< minutes.count() << " minutes, "
			<< seconds.count() << " seconds " << std::endl;
	}
}



#endif