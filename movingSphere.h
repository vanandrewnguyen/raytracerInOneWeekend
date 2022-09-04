#ifndef MOVINGSPHERE
#define MOVINGSPHERE

#include "hitable.h"

class Material;

class MovingSphere : public Hitable {
public:
	float radius;
	double time0, time1;
	vec3 center0, center1;
	vec3 colour;
	Material* matPtr;

public:
	// Constructor
	MovingSphere(vec3 origin, vec3 target, double _time0, double _time1, float rad, vec3 col, Material* mat);

	// Getters
	float getRadius();
	vec3 getCenter(double time) const;
	vec3 getColour();

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
};

MovingSphere::MovingSphere(vec3 origin, vec3 target, double _time0, double _time1, float rad, vec3 col, Material* mat) {
	radius = rad;
	center0 = origin;
	center1 = target;
	time0 = _time0;
	time1 = _time1;
	colour = col;
	matPtr = mat;
}

float MovingSphere::getRadius() {
	return radius;
}

vec3 MovingSphere::getCenter(double time) const {
	// Interpolate based on time
	// Get time from start / total time * distance 
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

vec3 MovingSphere::getColour() {
	return colour;
}

bool MovingSphere::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	vec3 originVec = r.getOrigin() - getCenter(r.getTime());
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
			rec.normal = (rec.pos - getCenter(r.getTime())) / radius;
			rec.matPtr = matPtr;
			return true;
		}
		quadratic = (-b + sqrt(b * b - a * c)) / a;
		if (quadratic < tMax && quadratic > tMin) {
			rec.t = quadratic;
			rec.pos = r.getPointParam(rec.t);
			rec.normal = (rec.pos - getCenter(r.getTime())) / radius;
			rec.matPtr = matPtr;
			return true;
		}
	}

	return false;
}

#endif