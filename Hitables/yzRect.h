#ifndef YZRECT_H
#define YZRECT_H

#include "hitable.h"
#include "../Materials/material.h"
#include "../Render/ray.h"

class YZRect : public Hitable {
public:
    std::shared_ptr<Material> matPtr;
    // k = y axis
    float y0, y1, z0, z1, k, area;

public:
    YZRect();
    YZRect(float _y0, float _y1, float _z0, float _z1, float k, std::shared_ptr<Material> mat);

    virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
    virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
};

YZRect::YZRect() {}

YZRect::YZRect(float _y0, float _y1, float _z0, float _z1, float _k, std::shared_ptr<Material> mat) {
    y0 = _y0;
    z0 = _z0;
    y1 = _y1;
    z1 = _z1;
    k = _k;
    matPtr = mat;
    area = std::fabs(y0 - y1) * std::fabs(z0 - z1);
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

bool YZRect::boundingBox(double _time0, double _time1, AABB& outputBox) const {
    float epsilon = 0.0001;
    // Bounding box needs non-zero width (give epsilon as small number)
    outputBox = AABB(vec3(k - epsilon, y0, z0), vec3(k + epsilon, y1, z1));
    return true;
}

#endif