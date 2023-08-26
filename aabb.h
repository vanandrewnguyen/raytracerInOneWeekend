#ifndef AABBHEADER
#define AABBHEADER

#include <cmath>
#include "interval.h"
#include "vec3.h"
#include "ray.h"

class AABB {
public:
	AABB();
	AABB(const Interval& ix, const Interval& iu, const Interval& iz);
	AABB(const vec3& a, const vec3& b);
	const Interval& axis(int num) const;
	bool hit(const Ray& r, Interval rayT) const;

public:
	Interval x, y, z;
};

AABB::AABB() {}

AABB::AABB(const Interval& ix, const Interval& iy, const Interval& iz) {
	x = ix;
	y = iy;
	z = iz;
}

AABB::AABB(const vec3& a, const vec3& b) {
	// a and b are extremes on the bounding box
	x = Interval(std::fmin(a[0], b[0]), std::fmax(a[0], b[0]));
	y = Interval(std::fmin(a[1], b[1]), std::fmax(a[1], b[1]));
	z = Interval(std::fmin(a[2], b[2]), std::fmax(a[2], b[2]));
}

const Interval& AABB::axis(int num) const {
	// Return corresponding axis
	if (num == 1) return y;
	if (num == 2) return z;
	return x;
}

bool AABB::hit(const Ray& r, Interval rayT) const {
	// Updated Intersection method by Andrew Kensler
	for (int a = 0; a < 3; a++) {
		auto invD = 1 / r.getDirection()[a];
		auto orig = r.getOrigin()[a];

		auto t0 = (axis(a).min - orig) * invD;
		auto t1 = (axis(a).max - orig) * invD;

		if (invD < 0) {
			std::swap(t0, t1);
		}

		if (t0 > rayT.min) {
			rayT.min = t0;
		}
		if (t1 < rayT.max) {
			rayT.max = t1;
		}

		if (rayT.max <= rayT.min) {
			return false;
		}
	}
	return true;


	/*
	// For three axis... calculate ray intersection
	for (int a = 0; a < 3; a++) {
		double t0 = std::fmin((axis(a).min - r.getOrigin()[a]) / r.getDirection()[a],
							  (axis(a).max - r.getOrigin()[a]) / r.getDirection()[a]);
		double t1 = std::fmax((axis(a).min - r.getOrigin()[a]) / r.getDirection()[a],
							  (axis(a).max - r.getOrigin()[a]) / r.getDirection()[a]);
		
		rayT.min = std::fmax(t0, rayT.min);
		rayT.max = std::fmin(t1, rayT.max);

		if (rayT.max <= rayT.min) {
			return false;
		}
	}
	return true;
	*/
}


#endif