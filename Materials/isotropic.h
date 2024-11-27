#ifndef ISOTROPIC_H
#define ISOTROPIC_H

// Mat that has properties same in all directions, like a constant volume of fog
#include "material.h"
#include "../Textures/texture.h"
#include "../Utils/spherePdf.h":

class Isotropic : public Material {
public:
	std::shared_ptr<raytrace::Texture> albedo;

public:
	Isotropic(std::shared_ptr<raytrace::Texture> a);

	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const override;
	virtual double scatteringPdf(const Ray& rayIn, const hitRecord& rec, const Ray& scatteredRay) const override;

};

Isotropic::Isotropic(std::shared_ptr<raytrace::Texture> a) {
	albedo = a;
}

bool Isotropic::scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const {
	/*
	scattered = Ray(rec.pos, randInUnitSphere(), rayIn.getTime());
	attenuation = albedo->getColourVal(rec.u, rec.v, rec.pos);
	return true;
	*/
	srec.attenuation = albedo->getColourVal(rec.u, rec.v, rec.pos);
	srec.pdfPtr = std::make_shared<SpherePdf>();
	srec.isSpecular = false;
	return true;
}

double Isotropic::scatteringPdf(const Ray& rayIn, const hitRecord& rec, const Ray& scatteredRay) const {
	return 1 / (4 * Utility::pi);
}

#endif
