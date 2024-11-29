#ifndef MOVINGSPHERE_H
#define MOVINGSPHERE_H

#include "hitable.h"

class Material;

class MovingSphere : public Hitable {
public:
	float radius;
	double time0, time1;
	vec3 center0, center1;
	vec3 colour;
	std::shared_ptr<Material> matPtr;

public:
	// Constructor
	MovingSphere(vec3 origin, vec3 target, double _time0, double _time1, float rad, vec3 col, std::shared_ptr<Material> mat);

	// Getters
	float getRadius();
	vec3 getCenter(double time) const;
	vec3 getColour();

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
	virtual double pdfValue(const vec3& origin, const vec3& direction) const override;
	virtual vec3 random(const vec3& origin) const override;
};

MovingSphere::MovingSphere(vec3 origin, vec3 target, double _time0, double _time1, float rad, vec3 col, std::shared_ptr<Material> mat) {
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

bool MovingSphere::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	vec3 radVec = vec3(radius, radius, radius);
	AABB box0(getCenter(_time0) - radVec, getCenter(_time0) + radVec);
	AABB box1(getCenter(_time1) - radVec, getCenter(_time1) + radVec);
	outputBox = AABB::surroundingBox(box0, box1);
	return true;
}

double MovingSphere::pdfValue(const vec3& origin, const vec3& direction) const {
	hitRecord rec;
	if (!this->hit(Ray(origin, direction), 0.001, Utility::infinity, rec))
		return 0;

	double distanceSquared = (rec.pos - origin).lengthSquared();
	double cosThetaMax = sqrt(1 - radius * radius / distanceSquared);
	double solidAngle = 2 * Utility::pi * (1 - cosThetaMax);

	return  1 / solidAngle;
}

vec3 MovingSphere::random(const vec3& origin) const {
	vec3 direction = getCenter(Utility::randomDouble(time0, time1)) - origin;
	auto disSquared = direction.lengthSquared();
	onb uvw;
	uvw.buildFromW(direction);
	return uvw.local(Utility::randomToSphere(radius, disSquared));

}

#endif