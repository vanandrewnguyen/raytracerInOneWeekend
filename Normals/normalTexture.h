#ifndef NORMALTEXTURE_H
#define NORMALTEXTURE_H

#include "normalBase.h"
#include "../Textures/texture.h"

class NormalTexture : public NormalBase {
public:
	NormalTexture();
	NormalTexture(float s, bool r);
	virtual ~NormalTexture() override;

	virtual vec3 computeNormalShift(const vec3& normal, float u, float v) override;
	void assignTexture(const std::shared_ptr<raytrace::Texture> &inputTex);

	void computeTangentSpace(const vec3& normal, float u, float v, vec3& tangent, vec3& bitangent) {
		// Assume UV mapping is consistent with spherical coordinates
		vec3 up = fabs(normal.getY()) < 0.99 ? vec3(0, 1, 0) : vec3(1, 0, 0);

		// Calculate tangent and bitangent vectors
		vec3 t = cross(up, normal);
		t.makeUnitVec();
		tangent = t;
		vec3 b = cross(normal, tangent);
		b.makeUnitVec();
		bitangent = b;
	}
public:
	double scale;
	bool reverse;
	bool hasTexture;
	std::shared_ptr<raytrace::Texture> texture;
};

NormalTexture::NormalTexture() {
	scale = 1.0;
	reverse = false;
	hasTexture = false;
	texture = nullptr;
}

NormalTexture::NormalTexture(float s, bool r) {
	scale = s;
	reverse = r;
	hasTexture = false;
	texture = nullptr;
}

NormalTexture::~NormalTexture() {

}

vec3 NormalTexture::computeNormalShift(const vec3& normal, float u, float v) {
	if (!hasTexture || !texture) {
		return vec3(0, 0, 0);
	}

	// RGB to tangent space perturbation
	// [0,255]->[-1,1] using perturbation = 2*col - 1
	vec3 texColor = texture->getColourVal(u, v, vec3(0, 0, 0));
	vec3 perturbation = 2.0 * texColor - vec3(1.0, 1.0, 1.0);
	perturbation *= scale;
	if (reverse) {
		perturbation *= -1;
	}

	vec3 tangent, bitangent;
	computeTangentSpace(normal, u, v, tangent, bitangent);

	// Convert tangent space perturbation to world space
	vec3 worldPerturbation = tangent * perturbation.getX() +
							 bitangent * perturbation.getY() +
							 normal * perturbation.getZ();

	return worldPerturbation;
}

void NormalTexture::assignTexture(const std::shared_ptr<raytrace::Texture>& inputTex) {
	texture = inputTex;
	hasTexture = true;
}

#endif