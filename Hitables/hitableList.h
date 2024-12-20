#ifndef HITABLELIST_H
#define HITABLELIST_H

#include "hitable.h"
#include "../Utils/utility.h"
#include <memory>
#include <vector>

class HitableList : public Hitable {
public:
	std::vector<std::shared_ptr<Hitable>> objectsList;

public:
	HitableList();
	HitableList(std::vector<std::shared_ptr<Hitable>>& list);

	void append(std::shared_ptr<Hitable> hitObject);
	std::vector<std::shared_ptr<Hitable>>& getList();

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
	virtual double pdfValue(const vec3& o, const vec3& v) const override;
	virtual vec3 random(const vec3& o) const override;
};

HitableList::HitableList() {}

HitableList::HitableList(std::vector<std::shared_ptr<Hitable>>& list) {
	objectsList = list;
}

void HitableList::append(std::shared_ptr<Hitable> hitObject) {
	objectsList.push_back(hitObject);
}

std::vector<std::shared_ptr<Hitable>>& HitableList::getList() {
	return objectsList;
}

bool HitableList::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	// Init struct
	hitRecord tempRec;

	bool hitAny = false;
	double closestDis = tMax;

	// Loop through every hitable item in the scene, and return the closests distance 
	// We don't care about things behind objects
	for (const std::shared_ptr<Hitable>& hitObject : objectsList) {
		if (hitObject->hit(r, tMin, closestDis, tempRec)) {
			hitAny = true;
			closestDis = tempRec.t;
			rec = tempRec;
		}
	}

	return hitAny;
}

bool HitableList::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	if (objectsList.size() == 0) {
		return false;
	}

	AABB tempBox;
	bool firstBox = true;

	for (const std::shared_ptr<Hitable>& hitObject : objectsList) {
		if (!(hitObject->boundingBox(_time0, _time1, tempBox))) {
			return false;
		}
		outputBox = firstBox ? tempBox : AABB::surroundingBox(outputBox, tempBox);
		firstBox = false;
	}

	return true;
}

double HitableList::pdfValue(const vec3& o, const vec3& v) const {
	auto weight = 1.0 / objectsList.size();
	auto sum = 0.0;

	for (const auto& object : objectsList)
		sum += weight * object->pdfValue(o, v);

	return sum;
}

vec3 HitableList::random(const vec3& o) const {
	auto int_size = static_cast<int>(objectsList.size());
	return objectsList[Utility::randomInt(0, int_size - 1)]->random(o);
}

#endif