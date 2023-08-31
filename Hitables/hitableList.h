#ifndef HITABLE_LIST
#define HITABLE_LIST

#include "hitable.h"
#include <memory>
#include <vector>

class HitableList : public Hitable {
public:
	std::vector<std::shared_ptr<Hitable>> thisList;

public:
	HitableList();
	HitableList(std::vector<std::shared_ptr<Hitable>>& list);

	void append(std::shared_ptr<Hitable> hitObject);
	std::vector<std::shared_ptr<Hitable>>& getList();

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

HitableList::HitableList() {}

HitableList::HitableList(std::vector<std::shared_ptr<Hitable>>& list) {
	thisList = list;
}

void HitableList::append(std::shared_ptr<Hitable> hitObject) {
	thisList.push_back(hitObject);
}

std::vector<std::shared_ptr<Hitable>>& HitableList::getList() {
	return thisList;
}

bool HitableList::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	// Init struct
	hitRecord tempRec;

	bool hitAny = false;
	double closestDis = tMax;

	// Loop through every hitable item in the scene, and return the closests distance 
	// We don't care about things behind objects
	for (const std::shared_ptr<Hitable>& hitObject : thisList) {
		if (hitObject->hit(r, tMin, closestDis, tempRec)) {
			hitAny = true;
			closestDis = tempRec.t;
			rec = tempRec;
		}
	}

	return hitAny;
}

bool HitableList::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	if (thisList.size() == 0) {
		return false;
	}

	AABB tempBox;
	bool firstBox = true;

	for (const std::shared_ptr<Hitable>& hitObject : thisList) {
		if (!(hitObject->boundingBox(_time0, _time1, tempBox))) {
			return false;
		}
		outputBox = firstBox ? tempBox : AABB::surroundingBox(outputBox, tempBox);
		firstBox = false;
	}

	return true;
}

#endif