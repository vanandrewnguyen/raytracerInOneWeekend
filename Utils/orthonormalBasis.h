#ifndef ONB_HEADER
#define ONB_HEADER

#include "vec3.h"
#include "utility.h"

class onb {
public:
    onb() {}

    inline vec3 operator[](int i) const {
        return axis[i];
    }

    vec3 u() const {
        return axis[0];
    }
    vec3 v() const {
        return axis[1];
    }
    vec3 w() const {
        return axis[2];
    }

    vec3 local(double a, double b, double c) const {
        return a * u() + b * v() + c * w();
    }

    vec3 local(const vec3& a) const {
        return a.getX() * u() + a.getY() * v() + a.getZ() * w();
    }

    void buildFromW(const vec3&);

public:
    vec3 axis[3];
};


void onb::buildFromW(const vec3& n) {
    axis[2] = unitVector(n);
    vec3 a = (fabs(w().getX()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    axis[1] = unitVector(cross(w(), a));
    axis[0] = cross(w(), v());
}


#endif