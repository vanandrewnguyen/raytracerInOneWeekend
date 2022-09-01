#ifndef CAMERA
#define CAMERA

#include "ray.h"

class Camera {
private:
    vec3 lowerLeftCorner;
    vec3 horizonatalVec;
    vec3 verticalVec;
    vec3 origin;
    float lensRadius;
    vec3 u, v, w;
public:
	Camera(vec3 lookFrom, vec3 lookAt, vec3 up, float vfov, float aspectRatio, float aperture, float focusDist);
    Ray getRay(float u, float v);
};

vec3 randomInUnitDisk() {
    vec3 p;
    do {
        // Z axis stays same, it's a disk
        p = 2.0 * vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

Camera::Camera(vec3 lookFrom, vec3 lookAt, vec3 up, float vfov, float aspectRatio, float aperture, float focusDist) {
    lensRadius = aperture / 2;
    float theta = vfov * Utility::pi / 180;
    float heightMid = tan(theta / 2);
    float widthMid = aspectRatio * heightMid;
    origin = lookFrom;
    w = unitVector(lookFrom - lookAt);
    u = unitVector(cross(up, w));
    v = cross(w, u);

    // Dis camera to scene is on z axis
    lowerLeftCorner = origin - widthMid * focusDist * u - heightMid * focusDist * v - focusDist * w;
    horizonatalVec = 2 * widthMid * u * focusDist;
    verticalVec = 2 * heightMid * v * focusDist;
}

Ray Camera::getRay(float s, float t) {
    vec3 rd = lensRadius * randomInUnitDisk();
    vec3 offset = u * rd.getX() + v * rd.getY();
    Ray rayDir((origin + offset), lowerLeftCorner + s * horizonatalVec + t * verticalVec - (origin + offset));
    return rayDir;
}




#endif