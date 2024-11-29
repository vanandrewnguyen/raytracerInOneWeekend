#ifndef METAL_H
#define METAL_H

#include "material.h"
#include "../Render/ray.h"
#include "../Hitables/hitable.h"

class MatMetal : public Material {
public:
	// Constructor
	MatMetal(const vec3& a, float r) {
		albedo = a;

		// Closer to 0 = more reflective
		reflectance = std::min(r, 1.0f);
	}

	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const {
	// virtual bool scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scatteredRay) const {
		/*
		// Scatter normal based on normal map
		vec3 normal = rec.normal;
		if (hasNormalMap) {
			std::shared_ptr<NormalBase> nmap = normalMap;
			vec3 perturbation = nmap->computeNormalShift(normal, rec.u, rec.v);
			normal = nmap->shiftNormal(normal, perturbation);
		}
		
		// Scatter our ray in random unit sphere
		vec3 reflectedRay = reflect(unitVector(rayIn.getDirection()), normal);
		scatteredRay = Ray(rec.pos, reflectedRay + (reflectance * randInUnitSphere()), rayIn.getTime());
		attenuation = albedo;
		return (dot(scatteredRay.getDirection(), normal) > 0);
		*/

		// Calculate the perturbed normal if a normal map is present
		vec3 normal = rec.normal;
		if (hasNormalMap) {
			std::shared_ptr<NormalBase> nmap = normalMap;
			vec3 perturbation = nmap->computeNormalShift(normal, rec.u, rec.v);
			normal = unitVector(nmap->shiftNormal(normal, perturbation));
		}

		vec3 reflected = reflect(unitVector(rayIn.getDirection()), normal);
		reflected = unitVector(reflected) + (reflectance * randomInUnitDisk());

		srec.specularRay = Ray(rec.pos, reflected, rayIn.getTime());
		srec.attenuation = albedo;
		srec.isSpecular = true;
		srec.pdfPtr = nullptr;

		return true;

	}
public:
	vec3 albedo;
	float reflectance;
};

#endif