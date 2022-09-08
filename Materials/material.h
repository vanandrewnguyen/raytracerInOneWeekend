#ifndef MATERIAL
#define MATERIAL

class Material {
public:
	// How rays react upon hitting material type
	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const = 0;

	// Default materials don't emit any light
	virtual vec3 emitted(float u, float v, const vec3& p) const {
		return vec3(0, 0, 0);
	}
};

#endif
