#ifndef CONSTANTVOLUME_H
#define CONSTANTVOLUME_H

#include "hitable.h"
#include "../Materials/material.h"
#include "../Utils/utility.h"
#include "../Materials/isotropic.h"

class ConstantVolume : public Hitable {
public:
	std::shared_ptr<Hitable> boundary;
	std::shared_ptr<Material> phaseFunc;
	double negInverseDensity;

public:
	ConstantVolume(std::shared_ptr<Hitable> hit, double density, std::shared_ptr<raytrace::Texture> tex);
	ConstantVolume(std::shared_ptr<Hitable> hit, double density, std::shared_ptr<Material> phase);
	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

ConstantVolume::ConstantVolume(std::shared_ptr<Hitable> hit, double density, std::shared_ptr<raytrace::Texture> tex) {
	boundary = hit;
	negInverseDensity = -1/density;
	phaseFunc = std::make_shared<Isotropic>(tex);
}

ConstantVolume::ConstantVolume(std::shared_ptr<Hitable> hit, double density, std::shared_ptr<Material> phase) {
	boundary = hit;
	negInverseDensity = -1 / density;
	phaseFunc = phase;
}

bool ConstantVolume::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	hitRecord rec1, rec2;

	if (!(boundary->hit(r, -Utility::infinity, Utility::infinity, rec1))) {
		return false;
	}

	if (!(boundary->hit(r, rec1.t + 0.0001, Utility::infinity, rec2))) {
		return false;
	}

	if (rec1.t < tMin) {
		rec1.t = tMin;
	}
	if (rec2.t > tMax) {
		rec2.t = tMax;
	}
	if (rec1.t >= rec2.t) {
		return false;
	}

	if (rec1.t < 0) {
		rec1.t = 0;
	}

	const float rayLen = r.getDirection().length();
	const float disInBoundary = (rec2.t - rec1.t) * rayLen;
	const float hitDis = negInverseDensity * log(Utility::randomDouble());

	if (hitDis > disInBoundary) {
		// We've exited the boundary
		return false;
	}

	rec.t = rec1.t + hitDis / rayLen;
	rec.pos = r.getPointParam(rec.t);

	// Normal's won't matter for a volume
	rec.normal = vec3(1, 0, 0);
	rec.frontFace = true;
	rec.matPtr = phaseFunc;

	return true;
}

bool ConstantVolume::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	return boundary->boundingBox(_time0, _time1, outputBox);
}

#endif