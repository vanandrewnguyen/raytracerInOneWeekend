#ifndef NORMALROUGH_H
#define NORMALROUGH_H

#include <random>
#include "normalBase.h"

class Rough : public NormalBase {
public:
	Rough();
	virtual ~Rough() override;

	virtual vec3 computeNormalShift(const vec3& normal, float u, float v) override;
public:
	std::shared_ptr<std::mt19937> randGen;
};

Rough::Rough() {
	std::random_device dev;
	std::seed_seq seed{ dev(), dev(), dev(), dev() };
	randGen = std::make_shared<std::mt19937>(seed);
}

Rough::~Rough() {

}

vec3 Rough::computeNormalShift(const vec3& normal, float u, float v) {
	std::uniform_real_distribution<float> randDist(-amplitude, amplitude);
	float x = randDist(*randGen);
	float y = randDist(*randGen);
	float z = randDist(*randGen);
	vec3 perturbation = vec3(x, y, z);
	return shiftNormal(normal, perturbation);
}

#endif