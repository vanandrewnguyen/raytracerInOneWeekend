#ifndef MIXTUREPDF_H
#define MIXTUREPDF_H

#include "pdf.h"

// Mixture density of cosine and light sampling
class MixturePdf : public Pdf {
public:
    MixturePdf(std::shared_ptr<Pdf> p0, std::shared_ptr<Pdf> p1) {
        p[0] = p0;
        p[1] = p1;
    }

    virtual double value(const vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    virtual vec3 generate() const override {
        return (Utility::randomDouble() < 0.5) ? p[0]->generate() : p[1]->generate();
    }

public:
    std::shared_ptr<Pdf> p[2];
};

#endif