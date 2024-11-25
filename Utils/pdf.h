#ifndef PDF_HEADER
#define PDF_HEADER

#include "orthonormalBasis.h"

class Pdf {
public:
    virtual ~Pdf() {}

    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

#endif