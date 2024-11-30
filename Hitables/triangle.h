#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hitable.h"
#include "../Utils/utility.h"

class Triangle : public Hitable {
public:
	vec3 v0, v1, v2;
	vec3 normal;
	std::shared_ptr<Material> matPtr;

public:
    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, std::shared_ptr<Material> mat);

    virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
    virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
    virtual double pdfValue(const vec3& o, const vec3& v) const override;
    virtual vec3 random(const vec3& o) const override;
};

Triangle::Triangle(const vec3& _v0, const vec3& _v1, const vec3& _v2, std::shared_ptr<Material> mat) {
    v0 = _v0;
    v1 = _v1;
    v2 = _v2;
    matPtr = mat;
    normal = unitVector(cross(v1 - v0, v2 - v0));
}

bool Triangle::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html
    // Moller Trumbore Ray Triangle Intersection
    const float epsilon = 1e-8;

    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    vec3 pvec = cross(r.getDirection(), v0v2);
    float determinant = dot(v0v1, pvec);

    // Parallel ray to triangle
    if (std::fabs(determinant) < epsilon) {
        return false;
    }

    float inverseDeterminant = 1.0 / determinant;
    vec3 tvec = r.getOrigin() - v0;
    float u = inverseDeterminant * dot(tvec, pvec);

    // Outside triangle
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    vec3 qvec = cross(tvec, v0v1);
    float v = inverseDeterminant * dot(r.getDirection(), qvec);

    // Outside triangle, u + v <= 1.0, both should be normalised
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    float t = inverseDeterminant * dot(v0v2, qvec);

    // Outside valid range
    if (t < tMin || t > tMax) {
        return false;
    }

    rec.t = t;
    rec.pos = r.getPointParam(t);
    rec.normal = normal;
    rec.matPtr = matPtr;
    return true;
}

bool Triangle::boundingBox(double _time0, double _time1, AABB& outputBox) const {
    // Compute bounding box, using min/max values of each xyz vert
    vec3 minPoint(fmin(fmin(v0.getX(), v1.getX()), v2.getX()),
                  fmin(fmin(v0.getY(), v1.getY()), v2.getY()),
                  fmin(fmin(v0.getZ(), v1.getZ()), v2.getZ()));

    vec3 maxPoint(fmax(fmax(v0.getX(), v1.getX()), v2.getX()),
                  fmax(fmax(v0.getY(), v1.getY()), v2.getY()),
                  fmax(fmax(v0.getZ(), v1.getZ()), v2.getZ()));

    outputBox = AABB(minPoint, maxPoint);
    return true;
}

double Triangle::pdfValue(const vec3& o, const vec3& v) const {
    hitRecord rec;
    if (!this->hit(Ray(o, v), 0.001, Utility::infinity, rec)) {
        return 0.0;
    }

    double area = 0.5 * cross(v1 - v0, v2 - v0).length();
    double distanceSquared = (rec.pos - o).lengthSquared();
    double cosine = fabs(dot(unitVector(v), normal));

    return distanceSquared / (cosine * area);
}

vec3 Triangle::random(const vec3& o) const {
    // Randomly sample a point on the triangle, any point can be expressed as P = A + uAB + vAC
    double u = sqrt(Utility::randomDouble());
    double v = Utility::randomDouble();
    vec3 randomPoint = (1 - u) * v0 + u * (1 - v) * v1 + u * v * v2;

    return unitVector(randomPoint - o);
}

#endif