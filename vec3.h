#include<math.h>
#include<stdlib.h>
#include<iostream>

class vec3 {
private:
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

	// Numerical operations
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