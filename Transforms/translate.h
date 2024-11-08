#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "../Hitables/hitable.h"
#include "../Render/ray.h"

class Translate : public Hitable {
public:
	std::shared_ptr<Hitable> hitPtr;
	vec3 offset;

public:
	Translate(std::shared_ptr<Hitable> p, const vec3& displacement);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

Translate::Translate(std::shared_ptr<Hitable> p, const vec3& displacement) {
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

bool Translate::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	if (!(hitPtr->boundingBox(_time0, _time1, outputBox))) {
		return false;
	}

	outputBox = AABB(outputBox.min() + offset, outputBox.max() + offset);
	return true;
}

#endif