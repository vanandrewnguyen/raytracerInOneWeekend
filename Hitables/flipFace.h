#ifndef FLIPFACE_H
#define FLIPFACE_H

#include "hitable.h"

class flipFace : public Hitable {
public:
    flipFace(std::shared_ptr<Hitable> p) : ptr(p) {}

    virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override {

        if (!ptr->hit(r, tMin, tMax, rec))
            return false;

        rec.frontFace = !rec.frontFace;
        return true;
    }

    virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override {
        return ptr->boundingBox(_time0, _time1, outputBox);
    }

public:
    std::shared_ptr<Hitable> ptr;
};

#endif