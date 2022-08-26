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
	Camera(vec3 lookFrom, vec3 lookAt, vec3 up, float vfov, float aspectRatio);
    Ray getRay(float u, float v);
};

Camera::Camera(vec3 lookFrom, vec3 lookAt, vec3 up, float vfov, float aspectRatio) {
    vec3 u, v, w;
    float theta = vfov * M_PI / 180;
    float heightMid = tan(theta / 2);
    float widthMid = aspectRatio * heightMid;
    origin = lookFrom;
    w = unitVector(lookFrom - lookAt);
    u = unitVector(cross(up, w));
    v = cross(w, u);

    // Dis camera to scene is on z axis
    lowerLeftCorner = vec3(-widthMid, -heightMid, -1.0);
    lowerLeftCorner = origin - widthMid * u - heightMid * v - w;
    horizonatalVec = 2 * widthMid * u;
    verticalVec = 2 * heightMid * v;
}

Ray Camera::getRay(float s, float t) {
    Ray rayDir(origin, lowerLeftCorner + s * horizonatalVec + t * verticalVec - origin);
    return rayDir;
}

#endif