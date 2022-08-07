#include "vec3.h"

class ray {
public:
	vec3 A, B;

public:
	// Constructors
	ray();
	ray(const vec3& a, const vec3 b);

	vec3 origin() const;
	vec3 direction() const;
	vec3 pointParam(float t) const;
};

ray::ray() {

}

ray::ray(const vec3& a, const vec3 b) {
	A = a;
	B = b;
}

vec3 ray::origin() const {
	return A;
}

vec3 ray::direction() const {
	return B;
}

vec3 ray::pointParam(float t) const {
	return A + t * B;
}