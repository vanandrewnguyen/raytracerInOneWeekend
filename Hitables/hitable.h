#ifndef HITABLE
#define HITABLE

#include "../ray.h"
#include "aabb.h"

class Material;

struct hitRecord {
	float t;
	float u;
	float v;
	vec3 pos;
	vec3 normal;
	Material* matPtr;

	inline void setSurfNormal(const Ray& r, const vec3& outwardNormal) {
		float frontFace = dot(r.getDirection(), outwardNormal);
		normal = frontFace < 0 ? outwardNormal : -outwardNormal;
	}
};

class Hitable {
public:
	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const = 0;
	// virtual AABB boundingBox() const = 0;
};

#endif