#ifndef RAY
#define RAY

#include "vec3.h"

class Ray {
private:
	vec3 A, B;

public:
	// Constructors
	Ray();
	Ray(const vec3& a, const vec3 b);

	vec3 getOrigin() const;
	vec3 getDirection() const;
	vec3 getPointParam(float t) const;
};

Ray::Ray() {

}

Ray::Ray(const vec3& a, const vec3 b) {
	A = a;
	B = b;
}

vec3 Ray::getOrigin() const {
	return A;
}

vec3 Ray::getDirection() const {
	return B;
}

vec3 Ray::getPointParam(float t) const {
	return A + t * B;
}

#endif