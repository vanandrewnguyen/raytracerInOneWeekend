#ifndef BOXH
#define BOXH

#include <memory>

#include "xyRect.h"
#include "xzRect.h"
#include "yzRect.h"
#include "../vec3.h"

class Box : public Hitable {
public:
	vec3 boxMin;
	vec3 boxMax;
	HitableList sides;

public:
	Box();
	Box(const vec3& p0, const vec3& p1, std::shared_ptr<Material> matPtr);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

Box::Box() {}

Box::Box(const vec3& p0, const vec3& p1, std::shared_ptr<Material> matPtr) {
	boxMin = p0;
	boxMax = p1;

	// Make sides
	HitableList list;
	list.append(std::make_shared<XYRect>(p0.getX(), p1.getX(), p0.getY(), p1.getY(), p1.getZ(), matPtr));
	list.append(std::make_shared<XYRect>(p0.getX(), p1.getX(), p0.getY(), p1.getY(), p0.getZ(), matPtr));

	list.append(std::make_shared<XZRect>(p0.getX(), p1.getX(), p0.getZ(), p1.getZ(), p1.getY(), matPtr));
	list.append(std::make_shared<XZRect>(p0.getX(), p1.getX(), p0.getZ(), p1.getZ(), p0.getY(), matPtr));

	list.append(std::make_shared<YZRect>(p0.getY(), p1.getY(), p0.getZ(), p1.getZ(), p1.getX(), matPtr));
	list.append(std::make_shared<YZRect>(p0.getY(), p1.getY(), p0.getZ(), p1.getZ(), p0.getX(), matPtr));

	sides = list;
}

bool Box::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	return sides.hit(r, tMin, tMax, rec);
}

bool Box::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	outputBox = AABB(boxMin, boxMax);
	return true;
}

#endif