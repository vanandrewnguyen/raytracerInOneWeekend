#ifndef DIFFUSELIGHT
#define DIFFUSELIGHT

#include "material.h"
#include "../Textures/texture.h"
#include "../ray.h"
#include "../Hitables/hitable.h"

class DiffuseLight : public Material {
public:
	std::shared_ptr<Texture> emit;
public:
	DiffuseLight(std::shared_ptr<Texture> tex);

	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const override;
	virtual vec3 emitted(float u, float v, const vec3& p) const override;
};

DiffuseLight::DiffuseLight(std::shared_ptr<Texture> tex) {
	emit = tex;
}

bool DiffuseLight::scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const {
	return false;
}

vec3 DiffuseLight::emitted(float u, float v, const vec3& p) const {
	return emit->getColourVal(u, v, p);
}

#endif