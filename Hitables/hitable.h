#ifndef HITABLE_H
#define HITABLE_H

#include <memory>

#include "aabb.h"
#include "../Render/ray.h"

class Material;

struct hitRecord {
	float t;
	float u;
	float v;
	vec3 pos;
	vec3 normal;
	std::shared_ptr<Material> matPtr;

	inline void setSurfNormal(const Ray& r, const vec3& outwardNormal) {
		float frontFace = dot(r.getDirection(), outwardNormal);
		normal = frontFace < 0 ? outwardNormal : -outwardNormal;
	}
};

class Hitable {
public:
	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const = 0;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const = 0;
};

#endif