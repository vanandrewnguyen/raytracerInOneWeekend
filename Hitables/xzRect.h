#ifndef XZRECT_H
#define XZRECT_H

#include "hitable.h"
#include "../Materials/material.h"
#include "../Render/ray.h"

class XZRect : public Hitable {
public:
    std::shared_ptr<Material> matPtr;
    // k = y axis
    float x0, x1, z0, z1, k;

public:
    XZRect();
    XZRect(float _x0, float _x1, float _z0, float _z1, float k, std::shared_ptr<Material> mat);

    virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
    virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

XZRect::XZRect() {}

XZRect::XZRect(float _x0, float _x1, float _z0, float _z1, float _k, std::shared_ptr<Material> mat) {
    x0 = _x0;
    z0 = _z0;
    x1 = _x1;
    z1 = _z1;
    k = _k;
    matPtr = mat;
}

bool XZRect::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
    float t = (k - r.getOrigin().getY()) / r.getDirection().getY();

    if (t < tMin || t > tMax) {
        return false;
    }

    float x = r.getOrigin().getX() + t * r.getDirection().getX();
    float z = r.getOrigin().getZ() + t * r.getDirection().getZ();

    if (x < x0 || x > x1 || z < z0 || z > z1) {
        return false;
    }

    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.matPtr = matPtr;
    rec.pos = r.getPointParam(t);
    rec.setSurfNormal(r, vec3(0, 1, 0));

    return true;
}

bool XZRect::boundingBox(double _time0, double _time1, AABB& outputBox) const {
    float epsilon = 0.0001;
    // Bounding box needs non-zero width (give epsilon as small number)
    outputBox = AABB(vec3(x0, k - epsilon, z0), vec3(x1, k + epsilon, z1));
    return true;
}

#endif