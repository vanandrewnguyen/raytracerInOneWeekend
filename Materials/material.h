#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "../Normals/normalBase.h"
#include "../Utils/orthonormalBasis.h"

class Material {
public:
	Material();

	// How rays react upon hitting material type
	virtual bool scatter(const Ray& rayIn, const hitRecord& rec, vec3& attenuation, Ray& scattered) const = 0;

	// Default materials don't emit any light
	virtual vec3 emitted(float u, float v, const vec3& p) const {
		return vec3(0, 0, 0);
	}

	void assignNormalMap(const std::shared_ptr<NormalBase> &inputMap);

public:
	bool hasNormalMap;
	std::shared_ptr<NormalBase> normalMap;
};

Material::Material() {
	hasNormalMap = false;
	normalMap = nullptr;
}

void Material::assignNormalMap(const std::shared_ptr<NormalBase>& inputMap) {
	hasNormalMap = true;
	normalMap = inputMap;
}

#endif
