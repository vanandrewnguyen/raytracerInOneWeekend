#ifndef TRANSLATE
#define TRANSLATE

#include "Hitables/hitable.h"
#include "ray.h"

class Translate : public Hitable {
public:
	Hitable* hitPtr;
	vec3 offset;

public:
	Translate(Hitable* p, const vec3& displacement);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
};

Translate::Translate(Hitable* p, const vec3& displacement) {
	hitPtr = p;
	offset = displacement;
}

bool Translate::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	Ray movedRay(r.getOrigin() - offset, r.getDirection(), r.getTime());
	if (!(hitPtr->hit(movedRay, tMin, tMax, rec))) {
		return false;
	}

	rec.pos += offset;
	rec.setSurfNormal(movedRay, rec.normal);

	return true;
}

#endif