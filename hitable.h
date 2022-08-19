#ifndef HITABLE
#define HITABLE

#include "ray.h"

class Material;

struct hitRecord {
	float t;
	vec3 pos;
	vec3 normal;
	Material* matPtr;
};

class Hitable {
public:
	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const = 0;
};

#endif