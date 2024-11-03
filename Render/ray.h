#ifndef RAY_H
#define RAY_H

#include "../Utils/vec3.h"

class Ray {
private:
	vec3 orig, dir;
	double tm;

public:
	// Constructors
	Ray();
	Ray(const vec3& a, const vec3& b, double time = 0.0);

	vec3 getOrigin() const;
	vec3 getDirection() const;
	vec3 getPointParam(float t) const;
	double getTime() const;
};

Ray::Ray() {

}

Ray::Ray(const vec3& a, const vec3& b, double time) {
	orig = a;
	dir = b;
	tm = time;
}

vec3 Ray::getOrigin() const {
	return orig;
}

vec3 Ray::getDirection() const {
	return dir;
}

double Ray::getTime() const {
	return tm;
}

vec3 Ray::getPointParam(float t) const {
	return orig + t * dir;
}

#endif