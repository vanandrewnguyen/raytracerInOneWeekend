#ifndef DIFFUSELIGHT_H
#define DIFFUSELIGHT_H

#include "material.h"
#include "../Textures/texture.h"
#include "../Render/ray.h"
#include "../Hitables/hitable.h"

class DiffuseLight : public Material {
public:
	 std::shared_ptr<raytrace::Texture> emit;

public:
	DiffuseLight(std::shared_ptr<raytrace::Texture> tex);

	// virtual bool scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const override;
	virtual vec3 emitted(const Ray& rayIn, const hitRecord& rec, float u, float v, const vec3& p) const override;
};

DiffuseLight::DiffuseLight(std::shared_ptr<raytrace::Texture> tex) {
	 emit = tex;
}

/*
bool DiffuseLight::scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const {
	return false;
}
*/

vec3 DiffuseLight::emitted(const Ray& rayIn, const hitRecord& rec, float u, float v, const vec3& p) const {
	return (rec.frontFace) ? emit->getColourVal(u, v, p) : vec3(0, 0, 0);
}

#endif