#ifndef METAL
#define METAL

#include "material.h"
#include "../ray.h"
#include "../hitable.h"

class MatMetal : public Material {
public:
	// Constructor
	MatMetal(const vec3& a, int r) {
		albedo = a;

		// Closer to 0 = more reflective
		reflectance = r;
		if (reflectance > 1) {
			reflectance = 1;
		}
	}

	// Change our scattering
	virtual bool scatter(const Ray& rayIn, const hitRecord rec, vec3& attenuation, Ray& scatteredRay) const {
		// Scatter our ray in random unit sphere
		vec3 reflectedRay = reflect(unitVector(rayIn.getDirection()), rec.normal);
		scatteredRay = Ray(rec.pos, reflectedRay + (reflectance * randInUnitSphere()));
		attenuation = albedo;
		return (dot(scatteredRay.getDirection(), rec.normal) > 0);
	}
public:
	vec3 albedo;
	int reflectance;
};

#endif