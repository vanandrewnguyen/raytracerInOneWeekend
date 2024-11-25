#ifndef SPHEREPDF_HEADER
#define SPHEREPDF_HEADER

#include "pdf.h"

class SpherePdf : public Pdf {
public:
    SpherePdf() {}

    double value(const vec3& direction) const override {
        return 1 / (4 * Utility::pi);
    }

    vec3 generate() const override {
        return randInUnitSphere();
    }
};

#endif