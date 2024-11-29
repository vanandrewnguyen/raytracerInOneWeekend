#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"
#include "../Utils/cosinePdf.h"


class MatLambertian : public Material {
public:
	std::shared_ptr<raytrace::Texture> albedoPtr;
public:
	// Constructor
	MatLambertian(const vec3& a, std::shared_ptr<raytrace::Texture> t) {
		albedoPtr = t;
	}
	MatLambertian(const vec3& a) {
		albedoPtr = std::make_shared<TexSolidColour>(a);
	}
	MatLambertian(std::shared_ptr<raytrace::Texture> t) {
		albedoPtr = t;
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

		// Scatter our ray in random unit sphere
		vec3 scatterDir = normal + randInUnitSphere();
		
		// Catch degenerate scatter direction (if you accidently generate the -normal vector, the sum will be zero hence math error)
		if (scatterDir.nearZero()) {
			scatterDir = normal;
		}
		
		scatteredRay = Ray(rec.pos, unitVector(scatterDir), rayIn.getTime());
		albedo = albedoPtr->getColourVal(rec.u, rec.v, rec.pos);
		pdf = dot(rec.normal, scatteredRay.getDirection()) / Utility::pi;
		*/
		/*
		onb uvw;
		uvw.buildFromW(rec.normal);
		vec3 direction = uvw.local(Utility::randomCosineDirection());
		scatteredRay = Ray(rec.pos, unitVector(direction), rayIn.getTime());

		albedo = albedoPtr->getColourVal(rec.u, rec.v, rec.pos);
		pdf = dot(uvw.w(), scatteredRay.getDirection()) / Utility::pi;
		*/

		// Calculate the perturbed normal if a normal map is present
		vec3 normal = rec.normal;
		if (hasNormalMap) {
			std::shared_ptr<NormalBase> nmap = normalMap;
			vec3 perturbation = nmap->computeNormalShift(normal, rec.u, rec.v);
			normal = unitVector(nmap->shiftNormal(normal, perturbation));
		}

		srec.isSpecular = false;
		srec.attenuation = albedoPtr->getColourVal(rec.u, rec.v, rec.pos);
		srec.pdfPtr = std::make_shared<CosinePdf>(normal);
		return true;
	}

	double scatteringPdf(const Ray& rayIn, const hitRecord& rec, const Ray& scatteredRay) const {
		auto cosine = dot(rec.normal, unitVector(scatteredRay.getDirection()));
		return cosine < 0 ? 0 : cosine / Utility::pi;
	}
};

#endif