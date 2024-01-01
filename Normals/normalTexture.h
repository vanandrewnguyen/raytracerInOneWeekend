#ifndef NORMALTEXTURE
#define NORMALTEXTURE

#include "normalBase.h"

class NormalTexture : public NormalBase {
public:
	NormalTexture();
	NormalTexture(float s, bool r);
	virtual ~NormalTexture() override;

	virtual vec3 computeNormalShift(const vec3& normal, float u, float v) override;
	void assignTexture(const std::shared_ptr<TexImage> &inputTex);
public:
	double scale;
	bool reverse;
	bool hasTexture;
	std::shared_ptr<TexImage> texture;
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
	return normal;
	/*
	* WIP
	float xD = 0.0;
	float yD = 0.0;
	float zD = 0.0;

	if (hasTexture) {
		// Modulo arithmatic to account for possible tiling
		u = fmod(u, 1.0);
		v = fmod(v, 1.0);

		// Convert (u,v) to image dimensions (x,y).
		double xsd = static_cast<float>(texture->getWidth());
		double ysd = static_cast<float>(texture->getHeight());
		double xF = ((u + 1.0) / 2.0) * xsd;
		double yF = ysd - (((v + 1.0) / 2.0) * ysd);
		int x = static_cast<int>(round(xF));
		int y = static_cast<int>(round(yF));
		int xMin = static_cast<int>(floor(xF));
		int yMin = static_cast<int>(floor(yF));
		int xMax = static_cast<int>(ceil(xF));
		int yMax = static_cast<int>(ceil(yF));

		// Perform bilinear interpolation.
		vec3 temp(0, 0, 0);
		vec3 col0 = texture->getColourVal(xMin, yMin, temp);
		vec3 col1 = texture->getColourVal(xMax, yMin, temp);
		vec3 col2 = texture->getColourVal(xMin, yMax, temp);
		vec3 col3 = texture->getColourVal(xMax, yMax, temp);

		double interpR = Utility::bilinearInterp(xMin, yMin, col0.getX(), xMax, yMin, col1.getX(), xMin, yMax, col2.getX(), xMax, yMax, col3.getX(), xF, yF);
		double interpG = Utility::bilinearInterp(xMin, yMin, col0.getY(), xMax, yMin, col1.getY(), xMin, yMax, col2.getY(), xMax, yMax, col3.getY(), xF, yF);
		double interpB = Utility::bilinearInterp(xMin, yMin, col0.getZ(), xMax, yMin, col1.getZ(), xMin, yMax, col2.getZ(), xMax, yMax, col3.getZ(), xF, yF);

		// Use the RGB values (ignore alpha) for the perturbation.
		xD = interpR;
		yD = interpG;
		zD = interpB;
	}

	vec3 perturbation = vec3(xD, yD, zD);
	return shiftNormal(normal, perturbation);
	*/

	/*
	
	
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
	*/
}

void NormalTexture::assignTexture(const std::shared_ptr<TexImage>& inputTex) {
	texture = inputTex;
	hasTexture = true;
}

#endif