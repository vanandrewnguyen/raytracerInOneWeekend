#ifndef UTILITY
#define UTILITY

#include <cmath>
#include <limits>
#include <memory>

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