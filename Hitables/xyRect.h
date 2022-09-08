#ifndef XYRECT
#define XYRECT

#include "hitable.h"
#include "../Materials/material.h"
#include "../ray.h"

class XYRect : public Hitable {
public:
	Material* matPtr;
    // k = z axis
	float x0, x1, y0, y1, k;

public:
	XYRect();
	XYRect(float _x0, float _x1, float _y0, float _y1, float k, Material* mat);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
};

XYRect::XYRect() {}

XYRect::XYRect(float _x0, float _x1, float _y0, float _y1, float _k, Material* mat) {
	x0 = _x0;
	y0 = _y0;
	x1 = _x1;
	y1 = _y1;
	k = _k;
	matPtr = mat;
}

bool XYRect::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
    float t = (k - r.getOrigin().getZ()) / r.getDirection().getZ();
    
    if (t < tMin || t > tMax) {
        return false;
    }
    
    float x = r.getOrigin().getX() + t * r.getDirection().getX();
    float y = r.getOrigin().getY() + t * r.getDirection().getY();
    if (x < x0 || x > x1 || y < y0 || y > y1) {
        return false;
    }
    
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    rec.matPtr = matPtr;
    rec.pos = r.getPointParam(t);
    rec.normal = vec3(0, 0, 1);
    
    return true;
}

#endif