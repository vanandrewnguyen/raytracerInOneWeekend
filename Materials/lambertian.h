#ifndef LAMBERTIAN
#define LAMBERTIAN

#include "material.h"

class MatLambertian : public Material {
public:
	// Constructor
	MatLambertian(const vec3& a) {
		albedo = a;
	}

	// Change our scattering
	virtual bool scatter(const Ray& rayIn, const hitRecord rec, vec3& attenuation, Ray& scatteredRay) const {
		// Scatter our ray in random unit sphere
		vec3 target = rec.pos + rec.normal + randInUnitSphere();
		scatteredRay = Ray(rec.pos, target - rec.pos);
		attenuation = albedo;
		return true;
	}
public:
	vec3 albedo; 
};

#endif