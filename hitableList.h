#ifndef HTIABLE_LIST
#define HITABLE_LIST

#include "hitable.h"

class HitableList : public Hitable {
private:
	Hitable **thisList;
	int thisListSize;
public:
	HitableList();
	HitableList(Hitable **list, int n);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
};

HitableList::HitableList() {
	thisList = NULL;
	thisListSize = 0;
}

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

#endif