#ifndef MATERIAL
#define MATERIAL

class Material {
public:
	// How rays react upon hitting material type
	virtual bool scatter(const Ray& rayIn, const hitRecord rec, vec3& attenuation, Ray& scattered) const = 0;
};

#endif
