#ifndef DIELECTRIC
#define DIELECTRIC

#include "material.h"
#include "../ray.h"
#include "../hitable.h"

class MatDielectric : public Material {
public:
	// Constructor
	MatDielectric(float ri) {
		refractionIndex = ri;
	}

	// Change our scattering
	virtual bool scatter(const Ray& rayIn, const hitRecord rec, vec3& attenuation, Ray& scatteredRay) const {
		vec3 outwardNormal;
		vec3 reflected = reflect(rayIn.getDirection(), rec.normal);
		float ior;

		// Default 
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflectProb;
		float cosine;

		if (dot(rayIn.getDirection(), rec.normal) > 0) {
			outwardNormal = -rec.normal;
			ior = refractionIndex;
			cosine = dot(rayIn.getDirection(), rec.normal) / rayIn.getDirection().length();
			cosine = sqrt(1 - refractionIndex * refractionIndex * (1 - cosine * cosine));
		} else {
			// If we're exiting the volume we need to do the calculations backwards, from e.g. glass back to air
			outwardNormal = rec.normal;
			ior = 1.0 / refractionIndex;
			cosine = -dot(rayIn.getDirection(), rec.normal) / rayIn.getDirection().length();
		}

		if (refract(rayIn.getDirection(), outwardNormal, ior, refracted)) {
			reflectProb = shlick(cosine, refractionIndex);
		} else {
			reflectProb = 1.0;
		}

		if (((float)rand() / RAND_MAX) < reflectProb) {
			scatteredRay = Ray(rec.pos, reflected);
		} else {
			scatteredRay = Ray(rec.pos, refracted);
		}

		return true;
	}
public:
	// IOR
	float refractionIndex;
};

#endif