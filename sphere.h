#ifndef SPHERE
#define SPHERE

#include "vec3.h"

class Sphere {
private:
	float radius;
	vec3 center;
	vec3 colour;

public:
	// Constructor
	Sphere(float rad, vec3 origin, vec3 col);

	// Getters
	float getRadius();
	vec3 getCenter();
	vec3 getColour();
};

Sphere::Sphere(float rad, vec3 origin, vec3 col) {
	radius = rad;
	center = origin;
	colour = col;
}

float Sphere::getRadius() {
	return radius;
}

vec3 Sphere::getCenter() {
	return center;
}

vec3 Sphere::getColour() {
	return colour;
}

#endif
