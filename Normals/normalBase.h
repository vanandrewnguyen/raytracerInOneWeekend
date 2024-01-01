#ifndef NORMALBASE
#define NORMALBASE

#include "../vec2.h"
#include "../vec3.h"
#include "../Textures/texture.h"

class NormalBase {
public:
	NormalBase();
	NormalBase(float a);
	virtual ~NormalBase();

	virtual vec3 computeNormalShift(const vec3& normal, float u, float v);
	vec3 shiftNormal(const vec3 &normal, const vec3 &perturbation);
	vec2 textureDiff(const std::shared_ptr<Texture> &inputTex, float u, float v);
	void setAmp(float a);
	
public:
	float amplitude;
};

NormalBase::NormalBase() {
	amplitude = 1.0;
}

NormalBase::NormalBase(float a) {
	amplitude = a;
}

NormalBase::~NormalBase() {

}

vec3 NormalBase::computeNormalShift(const vec3& normal, float u, float v) {
	// Default response, no perturbation
	return vec3(0, 0, 0);
}

vec3 NormalBase::shiftNormal(const vec3& normal, const vec3& perturbation) {
	// Get up vector
	vec3 up = vec3(0, 0, -1);
	if ((normal.getZ() > 0.99) || (normal.getZ() < -0.99)) {
		up = vec3(1, 0, 0);
	}

	// Compute direction based on tangent plane
	vec3 perturbationV = cross(up, normal);
	perturbationV.makeUnitVec();
	vec3 perturbationU = cross(normal, perturbationV);
	perturbationU.makeUnitVec();

	// Apply perturbation
	vec3 res = normal + (perturbationU * perturbation.getX()) + (perturbationV * perturbation.getY()) + (normal * perturbation.getZ());
	res.makeUnitVec();
	return res;
}

vec2 NormalBase::textureDiff(const std::shared_ptr<Texture>& inputTex, float u, float v) {
	// Get partial derivatives of texture (slope grad) at (u, v)
	float ep = 0.001;
	vec2 uDisplacement = vec2(ep, 0.0);
	vec2 vDisplacement = vec2(0.0, ep);
	vec3 pos(0, 0, 0);
	float uDelta = ((inputTex->getVal(u + uDisplacement.getX(), v + uDisplacement.getY())) - 
					(inputTex->getVal(u + uDisplacement.getX(), v + uDisplacement.getY()))) / (2.0 * ep);
	float vDelta = ((inputTex->getVal(u + vDisplacement.getX(), v + vDisplacement.getY())) -
					(inputTex->getVal(u + vDisplacement.getX(), v + vDisplacement.getY()))) / (2.0 * ep);

	return vec2(uDelta, vDelta);
}

void NormalBase::setAmp(float a) {
	amplitude = a;
}

#endif