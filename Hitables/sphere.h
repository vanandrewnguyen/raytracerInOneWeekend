#ifndef SPHERE
#define SPHERE

#include "hitable.h"
#include "../utility.h"

class Material;

class Sphere: public Hitable {
public:
	float radius;
	vec3 center;
	vec3 colour;
	Material* matPtr;

public:
	// Constructor
	Sphere(float rad, vec3 origin, vec3 col, Material* mat);

	// Getters
	float getRadius();
	vec3 getCenter();
	vec3 getColour();
	static void getUV(const vec3& p, float& u, float& v);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
};

Sphere::Sphere(float rad, vec3 origin, vec3 col, Material* mat) {
	radius = rad;
	center = origin;
	colour = col;
	matPtr = mat;
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
	float b = dot(originVec, r.getDirection());
	float c = dot(originVec, originVec) - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant > 0) {
		// Two solutions, isn't elegant at all 
		float quadratic = (-b - sqrt(b * b - a * c)) / a;
		if (quadratic < tMax && quadratic > tMin) {
			rec.t = quadratic;
			rec.pos = r.getPointParam(rec.t);
			rec.normal = (rec.pos - center) / radius;
			rec.matPtr = matPtr;
			getUV(rec.normal, rec.u, rec.v);
			return true;
		}
		quadratic = (-b + sqrt(b * b - a * c)) / a;
		if (quadratic < tMax && quadratic > tMin) {
			rec.t = quadratic;
			rec.pos = r.getPointParam(rec.t);
			rec.normal = (rec.pos - center) / radius;
			rec.matPtr = matPtr;
			getUV(rec.normal, rec.u, rec.v);
			return true;
		}
	}

	return false;
}

void Sphere::getUV(const vec3& p, float& u, float& v) {
	// p = point on sphere, u, v [0, 1] of angle from: Y axis from x = -1 and Y = -1 -> Y = 1
	float theta = acos(-p.getY());
	float phi = atan2(-p.getZ(), p.getX()) + Utility::pi;

	u = phi / (2 * Utility::pi);
	v = theta / phi;
}

#endif
