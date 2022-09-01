#ifndef VEC3
#define VEC3

#include<math.h>
#include<stdlib.h>
#include<iostream>

class vec3 {
public:
	float e[3];

public:
	// Constructors
	vec3();
	vec3(float x, float y, float z);

	// Getters
	inline float getX() const { return e[0]; }
	inline float getY() const { return e[1]; }
	inline float getZ() const { return e[2]; }

	// Def vector operators
	inline const vec3& operator+() const { return *this; }
	inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	inline float operator[](int i) const { return e[i]; }
	inline float& operator[](int i) { return e[i]; }

	// Scalar operations
	inline vec3& operator+=(const vec3& v2);
	inline vec3& operator-=(const vec3& v2);
	inline vec3& operator*=(const vec3& v2);
	inline vec3& operator/=(const vec3& v2);
	inline vec3& operator*=(const float t);
	//inline vec3& operator/=(const float t);
	inline vec3& operator/=(const double t) { return *this *= 1 / t; }

	// Basic math operations
	inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	inline float lengthSquared() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	void makeUnitVec();
	bool nearZero() const;
};

// Constructor
vec3::vec3() {}

vec3::vec3(float x, float y, float z) {
	e[0] = x;
	e[1] = y;
	e[2] = z;
}

// Redirection io streams
inline std::istream& operator>>(std::istream& is, vec3& t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}

inline std::ostream& operator<<(std::ostream& os, vec3& t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}

inline void vec3::makeUnitVec() {
	// Get 1 / len and multiply all the components by the new magnitude
	float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	e[0] *= k; 
	e[1] *= k; 
	e[2] *= k;
}

// Change def vector numerical operations
inline vec3 operator+(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
inline vec3 operator-(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}
inline vec3 operator*(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}
inline vec3 operator/(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}
inline vec3 operator*(float t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline vec3 operator*(const vec3& v, float t) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline vec3 operator/(float t, const vec3& v) {
	return vec3(t / v.e[0], t / v.e[1], t / v.e[2]);
}
inline vec3 operator/(const vec3& v, float t) {
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

// Vector and vector operations
inline vec3& vec3::operator+=(const vec3& v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}
inline vec3& vec3::operator-=(const vec3& v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}
inline vec3& vec3::operator*=(const vec3& v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}
inline vec3& vec3::operator/=(const vec3& v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}
inline vec3& vec3::operator*=(const float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

/*
inline vec3& vec3::operator/=(const float t) {
	float k = 1.0 / t;
	e[0] /= k;
	e[1] /= k;
	e[2] /= k;
	return *this;
}*/

// Dot and Cross products
inline float dot(const vec3 & v1, const vec3 & v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline vec3 cross(const vec3 & v1, const vec3 & v2) {
	return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2[1],
		-(v1.e[0] * v2.e[2] - v1.e[2] * v2[0]),
		v1.e[0] * v2.e[1] - v1.e[1] * v2[0]);
}

// Get unit vector
inline vec3 unitVector(vec3 v) {
	return v / v.length();
}

// Random vector of 0->1 length
vec3 randInUnitSphere() {
	vec3 p;
	do {
		p = 2.9 * vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX) - vec3(1, 1, 1);
	} while (p.lengthSquared() >= 1.0);
	return p;
}

// Reflect and refract functions for vector
vec3 reflect(const vec3& vector, const vec3& normal) {
	return vector - 2 * dot(vector, normal) * normal;
}

bool refract(const vec3& vector, const vec3& normal, float ior, vec3& refractedVector) {
	vec3 uv = unitVector(vector);
	float dt = dot(uv, normal);
	float discriminant = 1.0 - ior * ior * (1 - dt * dt);
	if (discriminant > 0) {
		refractedVector = ior * (uv - normal * dt) - normal * sqrt(discriminant);
		return true;
	}

	return false;
}

// Shlick approx
float shlick(float cosine, float ior) {
	float r0 = (1 - ior) / (1 + ior);
	r0 = r0 * r0;
	return r0 + (1 - r0) * (pow((1 - cosine), 5));
}

bool vec3::nearZero() const {
	// Return true if the vector is close to zero in all dimensions.
	const auto s = 1e-8;
	return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
}

#endif