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
	inline vec3& operator/=(const float t);

	// Basic math operations
	inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	inline float lengthSquared() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	void makeUnitVec();
};

// Constructor
vec3::vec3() {

}

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
inline vec3 operator/(float t, const vec3& v) {
	return vec3(t / v.e[0], t / v.e[1], t / v.e[2]);
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
inline vec3& vec3::operator/=(const float t) {
	float k = 1.0 / t;
	e[0] /= k;
	e[1] /= k;
	e[2] /= k;
	return *this;
}

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

