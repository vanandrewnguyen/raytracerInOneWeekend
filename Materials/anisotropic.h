#ifndef ANISOTROPIC_H
#define ANISOTROPIC_H

#include "material.h"
#include "../Textures/texture.h"
#include "../Utils/spherePdf.h"
#include "../Utils/perlin.h"

class DensityField {
public:
    virtual double getDensity(const vec3& point) const = 0;

public:
    double threshold;
};

class PerlinDensityField : public DensityField {
public:
    PerlinDensityField(double scale = 6.0, double threshold = 0.2)
        : scale(scale), threshold(threshold) {
    }

    virtual double getDensity(const vec3& point) const override {
        double value = noise.fbm(point * scale, 3);
        return value > threshold ? value : 0.0;
    }

public:
    Perlin noise;
    double scale;
    double threshold;
};

class Anisotropic : public Material {
public:
    std::shared_ptr<raytrace::Texture> albedo;
    std::shared_ptr<DensityField> densityField;

public:
    Anisotropic(std::shared_ptr<raytrace::Texture> a, std::shared_ptr<DensityField> dField)
        : albedo(a), densityField(dField) {
    }

    virtual bool scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const override;
    virtual double scatteringPdf(const Ray& rayIn, const hitRecord& rec, const Ray& scatteredRay) const override;
};

bool Anisotropic::scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const {
    // Get density from the field at the hit point
    double density = densityField->getDensity(rec.pos);

    if (density <= 0.01) {
        return false;
    }

    // Calculate attenuation based on density and texture
    srec.attenuation = albedo->getColourVal(rec.u, rec.v, rec.pos) * (1.0 / density);
    srec.pdfPtr = std::make_shared<SpherePdf>();
    srec.isSpecular = false;
    return true;
}

double Anisotropic::scatteringPdf(const Ray& rayIn, const hitRecord& rec, const Ray& scatteredRay) const {
    // Scattering probability scales with density
    double density = densityField->getDensity(rec.pos);
    return density > densityField->threshold ? density / (4 * Utility::pi) : 0.0;
}

#endif