#ifndef ROTATE
#define ROTATE

#include "Hitables/hitable.h"
#include "ray.h"
#include "utility.h"
#include <limits>

class RotateY : public Hitable {
public:
	Hitable* hitPtr;
	float sineTheta;
	float cosineTheta;
	bool hasBox;
	AABB bbox;

public:
	RotateY(Hitable* hit, float angle);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

RotateY::RotateY(Hitable* hit, float angle) {
	hitPtr = hit;

	float rad = Utility::degToRad(angle);
	sineTheta = sin(rad);
	cosineTheta = cos(rad);

	// BBOX
	hasBox = hitPtr->boundingBox(0, 1, bbox);
	float inf = std::numeric_limits<float>::infinity();
	vec3 min = vec3(inf, inf, inf);
	vec3 max = vec3(-inf, -inf, -inf);

	// Switch between 3 axis for rotation across axis
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().getX() + (1 - i) * bbox.min().getX();
				auto y = j * bbox.max().getY() + (1 - j) * bbox.min().getY();
				auto z = k * bbox.max().getZ() + (1 - k) * bbox.min().getZ();

				auto newx = cosineTheta * x + sineTheta * z;
				auto newz = -sineTheta * x + cosineTheta * z;

				vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = AABB(min, max);
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

bool RotateY::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	outputBox = bbox;
	return hasBox;
}

#endif