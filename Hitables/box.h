#ifndef BOXH
#define BOXH

#include "xyRect.h"
#include "xzRect.h"
#include "yzRect.h"
#include "../vec3.h"

class Box : public Hitable {
public:
	vec3 boxMin;
	vec3 boxMax;
	HitableList* sides;

public:
	Box();
	Box(const vec3& p0, const vec3& p1, Material* matPtr);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
};

Box::Box() {}

Box::Box(const vec3& p0, const vec3& p1, Material* matPtr) {
	boxMin = p0;
	boxMax = p1;

	// Make sides
	Hitable** list = new Hitable * [6];
	list[0] = new XYRect(p0.getX(), p1.getX(), p0.getY(), p1.getY(), p1.getZ(), matPtr);
	list[1] = new XYRect(p0.getX(), p1.getX(), p0.getY(), p1.getY(), p0.getZ(), matPtr);

	list[2] = new XZRect(p0.getX(), p1.getX(), p0.getZ(), p1.getZ(), p1.getY(), matPtr);
	list[3] = new XZRect(p0.getX(), p1.getX(), p0.getZ(), p1.getZ(), p0.getY(), matPtr);

	list[4] = new YZRect(p0.getY(), p1.getY(), p0.getZ(), p1.getZ(), p1.getX(), matPtr);
	list[5] = new YZRect(p0.getY(), p1.getY(), p0.getZ(), p1.getZ(), p0.getX(), matPtr);

	sides = new HitableList(list, 6);
}

bool Box::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	return sides->hit(r, tMin, tMax, rec);
}


#endif