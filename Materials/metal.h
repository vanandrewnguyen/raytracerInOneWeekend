#ifndef METAL
#define METAL

#include "material.h"
#include "../ray.h"
#include "../hitable.h"

class MatMetal : public Material {
public:
	// Constructor
	MatMetal(const vec3& a) {
		albedo = a;
	}

	// Change our scattering
	virtual bool scatter(const Ray& rayIn, const hitRecord rec, vec3& attenuation, Ray& scatteredRay) const {
		// Scatter our ray in random unit sphere
		vec3 reflectedRay = reflect(unitVector(rayIn.getDirection()), rec.normal);
		scatteredRay = Ray(rec.pos, reflectedRay);
		attenuation = albedo;
		return (dot(scatteredRay.getDirection(), rec.normal) > 0);
	}
public:
	vec3 albedo;
};

#endif