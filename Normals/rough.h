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
	std::shared_ptr<std::normal_distribution<float>> dist;
};

Rough::Rough() {
	std::random_device dev;
	std::seed_seq seed{ dev(), dev(), dev(), dev() };
	randGen = std::make_shared<std::mt19937>(seed);
	dist = std::make_shared<std::normal_distribution<float>>(0.0f, 1.0f);
}

Rough::~Rough() {

}

vec3 Rough::computeNormalShift(const vec3& normal, float u, float v) {
	float dx = (*dist)(*randGen) * amplitude;
	float dy = (*dist)(*randGen) * amplitude;
	float dz = (*dist)(*randGen) * amplitude;
	return vec3(dx, dy, dz);
}

#endif