#ifndef COSINEPDF_HEADER
#define COSINEPDF_HEADER

#include "pdf.h"

class CosinePdf : public Pdf {
public:
    CosinePdf(const vec3& w) : uvw(w) {}

    double value(const vec3& direction) const override {
        auto cosine_theta = dot(unitVector(direction), uvw.w());
        return std::fmax(0, cosine_theta / Utility::pi);
    }

    vec3 generate() const override {
        return uvw.transform(Utility::randomCosineDirection());
    }

private:
    Onb uvw;
};
#endif