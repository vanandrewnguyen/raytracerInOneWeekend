#ifndef HTIABLE_LIST
#define HITABLE_LIST

#include "hitable.h"

class HitableList : public Hitable {
public:
	Hitable** thisList;
	int thisListSize;

public:
	HitableList();
	HitableList(Hitable** list, int n);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

HitableList::HitableList() {}

HitableList::HitableList(Hitable** list, int n) {
	thisList = list;
	thisListSize = n;
}

bool HitableList::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	// Init struct
	hitRecord tempRec;

	bool hitAny = false;
	double closestDis = tMax;

	// Loop through every hitable item in the scene, and return the closests distance 
	// We don't care about things behind objects
	for (int i = 0; i < thisListSize; i++) {
		if (thisList[i]->hit(r, tMin, closestDis, tempRec)) {
			hitAny = true;
			closestDis = tempRec.t;
			rec = tempRec;
		}
	}

	return hitAny;
}

bool HitableList::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	if (thisListSize == 0) {
		return false;
	}

	AABB tempBox;
	bool firstBox = true;

	for (int i = 0; i < thisListSize; i++) {
		if (thisList[i] != nullptr) {
			if (!(thisList[i]->boundingBox(_time0, _time1, tempBox))) {
				return false;
			}
			outputBox = firstBox ? tempBox : AABB::surroundingBox(outputBox, tempBox);
			firstBox = false;
		}
	}

	return true;
}

#endif