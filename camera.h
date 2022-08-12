#ifndef CAMERA
#define CAMERA

#include "ray.h"

class Camera {
private:
    vec3 lowerLeftCorner;
    vec3 horizonatalVec;
    vec3 verticalVec;
    vec3 origin;
public:
	Camera();
    Ray getRay(float u, float v);
};

Camera::Camera() {
    lowerLeftCorner = vec3(-2.0, -1.0, -1.0);
    horizonatalVec = vec3(4.0, 0.0, 0.0);
    verticalVec = vec3(0.0, 2.0, 0.0);
    origin = vec3(0.0, 0.0, 0.0);
}

Ray Camera::getRay(float u, float v) {
    Ray rayDir(origin, lowerLeftCorner + u * horizonatalVec + v * verticalVec - origin);
    return rayDir;
}

#endif