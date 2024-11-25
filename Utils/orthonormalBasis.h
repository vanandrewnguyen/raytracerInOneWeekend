#ifndef ONB_HEADER
#define ONB_HEADER

#include "vec3.h"
#include "utility.h"

class Onb {
public:
    Onb(const vec3& n) {
        axis[2] = unitVector(n);
        vec3 a = (std::fabs(axis[2].getX()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
        axis[1] = unitVector(cross(axis[2], a));
        axis[0] = cross(axis[2], axis[1]);
    }

    const vec3& u() const { return axis[0]; }
    const vec3& v() const { return axis[1]; }
    const vec3& w() const { return axis[2]; }

    vec3 transform(const vec3& v) const {
        // Transform from basis coordinates to local space.
        return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
    }

private:
    vec3 axis[3];
};


#endif