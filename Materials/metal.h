#ifndef METAL
#define METAL

#include "material.h"
#include "../ray.h"
#include "../Hitables/hitable.h"

class MatMetal : public Material {
public:
	// Constructor
	MatMetal(const vec3& a, float r) {
		albedo = a;

		// Closer to 0 = more reflective
		reflectance = r;
		if (reflectance > 1) {
			reflectance = 1;
		}
	}

	// Change our scattering
	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scatteredRay) const {
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
	}
public:
	vec3 albedo;
	float reflectance;
};

#endif