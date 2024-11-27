#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "material.h"
#include "../Render/ray.h"
#include "../Hitables/hitable.h"

class MatDielectric : public Material {
public:
	// Constructor
	MatDielectric(float ri) {
		refractionIndex = ri;
	}

	// Change our scattering
	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const {
		/*
		// Scatter normal based on normal map
		vec3 normal = rec.normal;
		if (hasNormalMap) {
			std::shared_ptr<NormalBase> nmap = normalMap;
			vec3 perturbation = nmap->computeNormalShift(normal, rec.u, rec.v);
			normal = nmap->shiftNormal(normal, perturbation);
		}
		
		vec3 outwardNormal;
		vec3 reflected = reflect(rayIn.getDirection(), normal);
		float ior;

		// Default 
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflectProb;
		float cosine;

		if (dot(rayIn.getDirection(), normal) > 0) {
			outwardNormal = -normal;
			ior = refractionIndex;
			cosine = dot(rayIn.getDirection(), normal) / rayIn.getDirection().length();
			cosine = sqrt(1 - refractionIndex * refractionIndex * (1 - cosine * cosine));
		} else {
			// If we're exiting the volume we need to do the calculations backwards, from e.g. glass back to air
			outwardNormal = normal;
			ior = 1.0 / refractionIndex;
			cosine = -dot(rayIn.getDirection(), normal) / rayIn.getDirection().length();
		}

		if (refract(rayIn.getDirection(), outwardNormal, ior, refracted)) {
			reflectProb = shlick(cosine, refractionIndex);
		} else {
			reflectProb = 1.0;
		}

		if (((float)rand() / RAND_MAX) < reflectProb) {
			scatteredRay = Ray(rec.pos, reflected, rayIn.getTime());
		} else {
			scatteredRay = Ray(rec.pos, refracted, rayIn.getTime());
		}

		return true;
		*/
		srec.isSpecular = true;
		srec.pdfPtr = nullptr;
		srec.attenuation = vec3(1.0, 1.0, 1.0);
		double refractionRatio = rec.frontFace ? (1.0 / refractionIndex) : refractionIndex;

		// Scatter normal based on normal map
		vec3 normal = rec.normal;
		if (hasNormalMap) {
			std::shared_ptr<NormalBase> nmap = normalMap;
			vec3 perturbation = nmap->computeNormalShift(normal, rec.u, rec.v);
			normal = nmap->shiftNormal(normal, perturbation);
		}

		vec3 outwardNormal;
		vec3 reflected = reflect(rayIn.getDirection(), normal);
		float ior;

		// Default 
		vec3 refracted;
		float reflectProb;
		float cosine;

		if (dot(rayIn.getDirection(), normal) > 0) {
			outwardNormal = -normal;
			ior = refractionIndex;
			cosine = dot(rayIn.getDirection(), normal) / rayIn.getDirection().length();
			cosine = sqrt(1 - refractionIndex * refractionIndex * (1 - cosine * cosine));
		} else {
			// If we're exiting the volume we need to do the calculations backwards, from e.g. glass back to air
			outwardNormal = normal;
			ior = 1.0 / refractionIndex;
			cosine = -dot(rayIn.getDirection(), normal) / rayIn.getDirection().length();
		}

		reflectProb = (refract(rayIn.getDirection(), outwardNormal, ior, refracted)) ? shlick(cosine, refractionIndex) : 1.0;
		srec.specularRay = (((float)rand() / RAND_MAX) < reflectProb) ? Ray(rec.pos, reflected, rayIn.getTime()) : Ray(rec.pos, refracted, rayIn.getTime());
		return true;
	}
public:
	// IOR
	float refractionIndex;

private:
	static double reflectance(double cosine, double refractionIndex) {
		// Use Schlick's approximation for reflectance
		auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}

};

#endif