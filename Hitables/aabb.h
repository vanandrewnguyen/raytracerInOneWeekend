#ifndef AABBHEADER
#define AABBHEADER

#include <cmath>
#include "../vec3.h"
#include "../ray.h"

class AABB {
public:
	AABB();
	AABB(const vec3& a, const vec3& b);
	AABB(const AABB& box0, const AABB& box1);

    vec3 min() const;
    vec3 max() const;
	bool hit(const Ray& r, double tMin, double tMax) const;
    static AABB surroundingBox(AABB box0, AABB box1);

public:
	vec3 minimum, maximum;
};

AABB::AABB() {}

AABB::AABB(const vec3& a, const vec3& b) {
    minimum = a;
    maximum = b;
}

AABB::AABB(const AABB& box0, const AABB& box1) {
	
}

vec3 AABB::min() const {
    return minimum;
}

vec3 AABB::max() const {
    return maximum;
}

bool AABB::hit(const Ray& r, double tMin, double tMax) const {
	// Updated Intersection method by Andrew Kensler
	// For each of 3 axis, calculate bounds
    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.getDirection()[a];
        auto t0 = (min()[a] - r.getOrigin()[a]) * invD;
        auto t1 = (max()[a] - r.getOrigin()[a]) * invD;
        if (invD < 0.0f) {
            std::swap(t0, t1);
        }

        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;

        if (tMax <= tMin) {
            return false;
        }
    }
    return true;
}

AABB AABB::surroundingBox(AABB box0, AABB box1) {
    vec3 smallPoint(std::fmin(box0.min().getX(), box1.min().getX()),
                    std::fmin(box0.min().getY(), box1.min().getY()),
                    std::fmin(box0.min().getZ(), box1.min().getZ()));

    vec3 bigPoint(std::fmax(box0.max().getX(), box1.max().getX()),
                  std::fmax(box0.max().getY(), box1.max().getY()),
                  std::fmax(box0.max().getZ(), box1.max().getZ()));

    return AABB(smallPoint, bigPoint);
}

#endif