#ifndef LAMBERTIAN
#define LAMBERTIAN

#include "material.h"
#include "../Textures/texture.h"

class MatLambertian : public Material {
public:
	vec3 albedo;
	std::shared_ptr<Texture> texPtr;
public:
	// Constructor
	MatLambertian(const vec3& a, std::shared_ptr<Texture> t = NULL) {
		albedo = a;
		texPtr = t;
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
		vec3 target = rec.pos + normal + randInUnitSphere();
		
		// Catch degenerate scatter direction (if you accidently generate the -normal vector, the sum will be zero hence math error)
		if (target.nearZero()) {
			target = normal;
		}
		
		scatteredRay = Ray(rec.pos, target - rec.pos, rayIn.getTime());
		if (texPtr != NULL) {
			attenuation = texPtr->getColourVal(rec.u, rec.v, rec.pos);
		} else {
			attenuation = albedo;
		}
		return true;
	}
};

#endif