#include <iostream>
#include "sdltemplate.h"
#include "ray.h"
#include "sphere.h"

bool sphereIntersection(Sphere s, const Ray& r) {
    float radius = s.getRadius();
    vec3 center = s.getCenter();

    // Remember the discriminant trick?? This is it
    vec3 originVec = r.getOrigin() - center;
    float a = dot(r.getDirection(), r.getDirection());
    float b = 2.0 * dot(originVec, r.getDirection());
    float c = dot(originVec, originVec) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

vec3 getSkyColour(const Ray& r) {
    vec3 unitDir = unitVector(r.getDirection());
    // Shift from -1->1 to 0->1
    float t = 0.5 * (unitDir.getY() + 1.0);

    // Interpolation
    vec3 whiteCol(1.0, 1.0, 1.0);
    vec3 skyCol(0.5, 0.7, 1.0);
    return (1.0 - t) * whiteCol + t * skyCol;
}

vec3 scene(const Ray& r) {
    Sphere testSphere(0.5, vec3(0.0, 0.0, -1.0), vec3(1.0, 0.0, 0.0));
    if (sphereIntersection(testSphere, r)) {
        return testSphere.getColour();
    }
    
    // If no collision, return sky colour
    return getSkyColour(r);
}

int main(int argc, char* argv[]) {
    const int imgWidth = 800;
    const int imgHeight = 400;

    // Establish SDL Window
    sdltemplate::sdl("Raytracer", imgWidth, imgHeight);
    sdltemplate::loop();

    // Set coordinate system (based on the width and height in ratio)
    vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
    vec3 horizonatalVec(4.0, 0.0, 0.0);
    vec3 verticalVec(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    // Bottom to top (img is reversed), left to right
    for (int y = imgHeight - 1; y >= 0; y--) {
        for (int x = 0; x < imgWidth; x++) {
            // Set UV's
            float u = float(x) / float(imgWidth);
            float v = float(y) / float(imgHeight);

            // Get sky colour
            Ray rayDir(origin, lowerLeftCorner + u * horizonatalVec + v * verticalVec);
            vec3 col = scene(rayDir);

            // Normalize values
            int ir = static_cast<int>(255.999 * col.getX());
            int ig = static_cast<int>(255.999 * col.getY());
            int ib = static_cast<int>(255.999 * col.getZ());

            // Output
            sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
            sdltemplate::drawPoint(x, imgHeight-y);
        }
    }
    
    // Keep window active
    while (sdltemplate::running) {
        sdltemplate::loop();
    }

    return 0;
}