#ifndef NORMALTEXTURE
#define NORMALTEXTURE

#include "normalBase.h"

class NormalTexture : public NormalBase {
public:
	NormalTexture();
	NormalTexture(float s, bool r);
	virtual ~NormalTexture() override;

	virtual vec3 computeNormalShift(const vec3& normal, float u, float v) override;
	void assignTexture(const std::shared_ptr<Texture> &inputTex);
public:
	double scale;
	bool reverse;
	bool hasTexture;
	std::shared_ptr<Texture> texture;
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
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;

	if (hasTexture) {
		vec2 uvGradient = textureDiff(texture, u, v);
		x = (reverse) ? uvGradient.getX() * scale : -uvGradient.getX() * scale;
		y = (reverse) ? uvGradient.getY() * scale : -uvGradient.getY() * scale;
	}

	vec3 shift = vec3(x, y, z);
	return shiftNormal(normal, shift);
}

void NormalTexture::assignTexture(const std::shared_ptr<Texture>& inputTex) {
	texture = inputTex;
	hasTexture = true;
}

#endif