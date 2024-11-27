#ifndef HITABLEPDF_H
#define HITABLEPDF_H

#include "pdf.h"
#include "../Hitables/hitable.h"

class HitablePdf : public Pdf {
public:
    HitablePdf(Hitable& objects, const vec3& origin) : objects(objects), origin(origin) {}

    virtual double value(const vec3& direction) const override {
        return objects.pdfValue(origin, direction);
    }

    virtual vec3 generate() const override {
        return objects.random(origin);
    }

public:
    vec3 origin;
    const Hitable& objects;
};

#endif