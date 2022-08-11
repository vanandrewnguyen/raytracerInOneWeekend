#ifndef SPHERE
#define SPHERE

#include "hitable.h"

class Sphere: public Hitable {
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

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
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

bool Sphere::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	vec3 originVec = r.getOrigin() - center;
	float a = dot(r.getDirection(), r.getDirection());
	float b = 2.0 * dot(originVec, r.getDirection());
	float c = dot(originVec, originVec) - radius * radius;
	float discriminant = b * b - 4 * a * c;

	if (discriminant > 0) {
		// Two solutions, isn't elegant at all 
		float quadratic = (-b - sqrt(b * b - a * c)) / a;
		if (quadratic < tMax && quadratic > tMin) {
			rec.t = quadratic;
			rec.pos = r.getPointParam(rec.t);
			rec.normal = (rec.pos - center) / radius;
			return true;
		}
		quadratic = (-b + sqrt(b * b - a * c)) / a;
		if (quadratic < tMax && quadratic > tMin) {
			rec.t = quadratic;
			rec.pos = r.getPointParam(rec.t);
			rec.normal = (rec.pos - center) / radius;
			return true;
		}
	}

	return false;
}

#endif
