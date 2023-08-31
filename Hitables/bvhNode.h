#ifndef BVH
#define BVH

#include "hitable.h"
#include "hitableList.h"
#include "../utility.h"

#include <algorithm>
#include <cstdio>
#include <memory>

class BVHNode : public Hitable {
public:
	BVHNode();
	BVHNode(std::vector<std::shared_ptr<Hitable>>& srcObjects, int start, int end, double _time0, double _time1);
	BVHNode(HitableList& list, double _time0, double _time1) : BVHNode(list.getList(), 0, list.getList().size(), _time0, _time1) {}

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const;
	virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
public:
	std::shared_ptr<Hitable> left;
	std::shared_ptr<Hitable> right;
	AABB bbox;
};

inline bool boxCompare(const std::shared_ptr<Hitable> a, const std::shared_ptr<Hitable> b, int axis) {
	AABB boxA;
	AABB boxB;

	if (!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
		std::cout << "No bounding box in bvh_node constructor." << std::endl;

	return boxA.min().e[axis] < boxB.min().e[axis];
}

bool boxXCompare(const std::shared_ptr<Hitable> a, const std::shared_ptr<Hitable> b) {
	return boxCompare(a, b, 0);
}

bool boxYCompare(const std::shared_ptr<Hitable> a, const std::shared_ptr<Hitable> b) {
	return boxCompare(a, b, 1);
}

bool boxZCompare(const std::shared_ptr<Hitable> a, const std::shared_ptr<Hitable> b) {
	return boxCompare(a, b, 2);
}

BVHNode::BVHNode() {}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hitable>>& srcObjects, int start, int end, double _time0, double _time1) {
	// Clone
	auto objects = srcObjects;
	
	// Use comparison function used by std::sort
	int axis = Utility::randomInt(0, 2);
	auto comparator = (axis == 0) ? boxXCompare
				    : (axis == 1) ? boxYCompare
					: boxZCompare;

	int objectSpan = end - start;

	if (objectSpan == 1) {
		left = right = objects[start];
	} else if (objectSpan == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		} else {
			left = objects[start + 1];
			right = objects[start];
		}
	} else {
		/*
		Need to make the change for hitables from Hitable** to std::vector...........
		
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		auto mid = start + objectSpan / 2;
		left = make_shared<bvh_node>(objects, start, mid, _time0, _time1);
		right = make_shared<bvh_node>(objects, mid, end, _time0, _time1);
		*/
	}

	AABB boxLeft, boxRight;

	if (!left->boundingBox(_time0, _time1, boxLeft)
		|| !right->boundingBox(_time0, _time1, boxRight)
		)
		std::cerr << "No bounding box in bvh_node constructor.\n";

	bbox = AABB::surroundingBox(boxLeft, boxRight);
}

bool BVHNode::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
	if (!bbox.hit(r, tMin, tMax)) {
		return false;
	}

	// Recurse through tree
	bool hitLeft = left->hit(r, tMin, tMax, rec);
	bool hitRight = right->hit(r, tMin, hitLeft ? rec.t : tMax, rec);

	return hitLeft || hitRight;
}

bool BVHNode::boundingBox(double _time0, double _time1, AABB& outputBox) const {
	outputBox = bbox;
	return true;
}

#endif