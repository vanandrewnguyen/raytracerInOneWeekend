#ifndef NORMALCONSTANT
#define NORMALCONSTANT

#include "normalBase.h"

class Constant : public NormalBase {
public:
	Constant();
	virtual ~Constant() override;

	virtual vec3 computeNormalShift(const vec3& normal, float u, float v) override;
public:
	vec3 displacement;
};

Constant::Constant() {

}

Constant::~Constant() {

}

vec3 Constant::computeNormalShift(const vec3& normal, float u, float v) {
	return normal;
}

#endif