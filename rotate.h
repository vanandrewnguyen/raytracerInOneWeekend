#ifndef ROTATE
#define ROTATE

#include "Hitables/hitable.h"
#include "ray.h"
#include "utility.h"

class RotateY : public Hitable {
public:
	Hitable* hitPtr;
	float sineTheta;
	float cosineTheta;

public:
	RotateY(Hitable* hit, float angle);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
};

RotateY::RotateY(Hitable* hit, float angle) {
	hitPtr = hit;

	float rad = Utility::degToRad(angle);
	sineTheta = sin(rad);
	cosineTheta = cos(rad);
}

bool RotateY::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	vec3 origin = r.getOrigin();
	vec3 dir = r.getDirection();

	// Remember the matrix
	origin[0] = cosineTheta * r.getOrigin()[0] - sineTheta * r.getOrigin()[2];
	origin[2] = sineTheta * r.getOrigin()[0] + cosineTheta * r.getOrigin()[2];

	dir[0] = cosineTheta * r.getDirection()[0] - sineTheta * r.getDirection()[2];
	dir[2] = sineTheta * r.getDirection()[0] + cosineTheta * r.getDirection()[2];

	// Make new ray from rotated ray
	Ray rotRay(origin, dir, r.getTime());

	if (!(hitPtr->hit(rotRay, tMin, tMax, rec))) {
		return false;
	}

	vec3 pos = rec.pos;
	vec3 normal = rec.normal;

	// Adjust position values and normals
	pos[0] = cosineTheta * rec.pos[0] + sineTheta * rec.pos[2];
	pos[2] = -sineTheta * rec.pos[0] + cosineTheta * rec.pos[2];
	normal[0] = cosineTheta * rec.normal[0] + sineTheta * rec.normal[2];
	normal[2] = -sineTheta * rec.normal[0] + cosineTheta * rec.normal[2];

	rec.pos = pos;
	rec.setSurfNormal(rotRay, normal);

	return true;
}

#endif