#ifndef XYRECT_H
#define XYRECT_H

#include "hitable.h"
#include "../Materials/material.h"
#include "../Render/ray.h"
#include "../Utils/interval.h"

class XYRect : public Hitable {
public:
    std::shared_ptr<Material> matPtr;
    // k = z axis
	float x0, x1, y0, y1, k, area;

public:
	XYRect();
	XYRect(float _x0, float _x1, float _y0, float _y1, float k, std::shared_ptr<Material> mat);

	virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
    virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
    virtual double pdfValue(const vec3& o, const vec3& v) const override;
    virtual vec3 random(const vec3& o) const override;
};

XYRect::XYRect() {}

XYRect::XYRect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<Material> mat) {
	x0 = _x0;
	y0 = _y0;
	x1 = _x1;
	y1 = _y1;
	k = _k;
	matPtr = mat;
    area = std::fabs(x0 - x1) * std::fabs(y0 - y1);
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
    rec.setSurfNormal(r, vec3(0, 0, 1));
    
    return true;
}

bool XYRect::boundingBox(double _time0, double _time1, AABB& outputBox) const {
    float epsilon = 0.0001;
    // Bounding box needs non-zero width (give epsilon as small number)
    outputBox = AABB(vec3(x0, y0, k - epsilon), vec3(x1, y1, k + epsilon));
    return true;
}

double XYRect::pdfValue(const vec3& origin, const vec3& v) const {
    hitRecord rec;
    if (!this->hit(Ray(origin, v), 0.001, Utility::infinity, rec)) {
        return 0;
    }

    double disSquared = rec.t * rec.t * v.lengthSquared();
    double cosine = fabs(dot(v, rec.normal) / v.length());

    return disSquared / (cosine * area);
}

vec3 XYRect::random(const vec3& origin) const {
    auto random_point = vec3(Utility::randomDouble(x0, x1), k, Utility::randomDouble(y0, y1));
    return random_point - origin;
}

#endif