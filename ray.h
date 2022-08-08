#include "vec3.h"

class ray {
public:
	vec3 A, B;

public:
	// Constructors
	ray();
	ray(const vec3& a, const vec3 b);

	vec3 getOrigin() const;
	vec3 getDirection() const;
	vec3 getPointParam(float t) const;
};

ray::ray() {

}

ray::ray(const vec3& a, const vec3 b) {
	A = a;
	B = b;
}

vec3 ray::getOrigin() const {
	return A;
}

vec3 ray::getDirection() const {
	return B;
}

vec3 ray::getPointParam(float t) const {
	return A + t * B;
}