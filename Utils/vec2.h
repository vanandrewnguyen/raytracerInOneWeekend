#ifndef VEC2_H
#define VEC2_H

#include<math.h>
#include<stdlib.h>
#include<iostream>

class vec2 {
public:
    float e[2];

public:
    // Constructors
    vec2();
    vec2(float x, float y);

    // Getters
    inline float getX() const { return e[0]; }
    inline float getY() const { return e[1]; }
    void setX(float val);
    void setY(float val);

    // Def vector operators
    inline const vec2& operator+() const { return *this; }
    inline vec2 operator-() const { return vec2(-e[0], -e[1]); }
    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }

    // Scalar operations
    inline vec2& operator+=(const vec2& v2);
    inline vec2& operator-=(const vec2& v2);
    inline vec2& operator*=(const vec2& v2);
    inline vec2& operator/=(const vec2& v2);
    inline vec2& operator*=(const float t);
    inline vec2& operator/=(const float t);

    // Basic math operations
    inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1]); }
    inline float lengthSquared() const { return e[0] * e[0] + e[1] * e[1]; }
    void makeUnitVec();
};

// Constructor
vec2::vec2() {}

vec2::vec2(float x, float y) {
    e[0] = x;
    e[1] = y;
}

// Redirection io streams
inline std::istream& operator>>(std::istream& is, vec2& t) {
    is >> t.e[0] >> t.e[1];
    return is;
}

inline std::ostream& operator<<(std::ostream& os, vec2& t) {
    os << t.e[0] << " " << t.e[1];
    return os;
}

inline void vec2::makeUnitVec() {
    float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1]);
    e[0] *= k;
    e[1] *= k;
}

// Change def vector numerical operations
inline vec2 operator+(const vec2& v1, const vec2& v2) {
    return vec2(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1]);
}
inline vec2 operator-(const vec2& v1, const vec2& v2) {
    return vec2(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1]);
}
inline vec2 operator*(const vec2& v1, const vec2& v2) {
    return vec2(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1]);
}
inline vec2 operator/(const vec2& v1, const vec2& v2) {
    return vec2(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1]);
}
inline vec2 operator*(float t, const vec2& v) {
    return vec2(t * v.e[0], t * v.e[1]);
}
inline vec2 operator*(const vec2& v, float t) {
    return vec2(t * v.e[0], t * v.e[1]);
}
inline vec2 operator/(float t, const vec2& v) {
    return vec2(t / v.e[0], t / v.e[1]);
}
inline vec2 operator/(const vec2& v, float t) {
    return vec2(v.e[0] / t, v.e[1] / t);
}

// Vector and vector operations
inline vec2& vec2::operator+=(const vec2& v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    return *this;
}
inline vec2& vec2::operator-=(const vec2& v) {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    return *this;
}
inline vec2& vec2::operator*=(const vec2& v) {
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    return *this;
}
inline vec2& vec2::operator/=(const vec2& v) {
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    return *this;
}
inline vec2& vec2::operator*=(const float t) {
    e[0] *= t;
    e[1] *= t;
    return *this;
}

inline vec2& vec2::operator/=(const float t) {
    float k = 1.0 / t;
    e[0] /= k;
    e[1] /= k;
    return *this;
}

// Dot products
inline float dot(const vec2& v1, const vec2& v2) {
    return v1[0] * v2[0] + v1[1] * v2[1];
}

// Get unit vector
inline vec2 unitVector(vec2 v) {
    return v / v.length();
}

#endif