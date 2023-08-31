#ifndef ISOTROPIC
#define ISOTROPIC

// Mat that has properties same in all directions, like a constant volume of fog
#include "material.h"
#include "../Textures/texture.h"

class Isotropic : public Material {
public:
	std::shared_ptr<Texture> albedo;

public:
	Isotropic(std::shared_ptr<Texture> a);

	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const override;

};

Isotropic::Isotropic(std::shared_ptr<Texture> a) {
	albedo = a;
}

bool Isotropic::scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const {
	scattered = Ray(rec.pos, randInUnitSphere(), rayIn.getTime());
	attenuation = albedo->getColourVal(rec.u, rec.v, rec.pos);
	return true;
}

#endif