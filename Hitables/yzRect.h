#ifndef YZRECT
#define YZRECT

#include "hitable.h"
#include "../Materials/material.h"
#include "../ray.h"

class YZRect : public Hitable {
public:
    Material* matPtr;
    // k = y axis
    float y0, y1, z0, z1, k;

public:
    YZRect();
    YZRect(float _y0, float _y1, float _z0, float _z1, float k, Material* mat);

    virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
};

YZRect::YZRect() {}

YZRect::YZRect(float _y0, float _y1, float _z0, float _z1, float _k, Material* mat) {
    y0 = _y0;
    z0 = _z0;
    y1 = _y1;
    z1 = _z1;
    k = _k;
    matPtr = mat;
}

bool YZRect::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
    float t = (k - r.getOrigin().getX()) / r.getDirection().getX();

    if (t < tMin || t > tMax) {
        return false;
    }

    float y = r.getOrigin().getY() + t * r.getDirection().getY();
    float z = r.getOrigin().getZ() + t * r.getDirection().getZ();

    if (y < y0 || y > y1 || z < z0 || z > z1) {
        return false;
    }

    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.matPtr = matPtr;
    rec.pos = r.getPointParam(t);
    rec.setSurfNormal(r, vec3(1, 0, 0)); 

    return true;
}
#endif