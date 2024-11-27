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
	bool frontFace;

	inline void setSurfNormal(const Ray& r, const vec3& outwardNormal) {
		frontFace = dot(r.getDirection(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class Hitable {
public:
	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const = 0;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const = 0;
	virtual double pdfValue(const vec3& o, const vec3& v) const {
		return 0.0;
	}
	virtual vec3 random(const vec3& o) const {
		return vec3(1, 0, 0);
	}
};

#endif