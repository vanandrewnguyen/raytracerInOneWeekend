#include <iostream>
#include "sdltemplate.h"
#include "ray.h"

vec3 colour(const ray& r) {
    vec3 unitDir = unitVector(r.getDirection());
    // Shift from -1->1 to 0->1
    float t = 0.5 * (unitDir.getY() + 1.0);

    // Interpolation
    vec3 whiteCol(1.0, 1.0, 1.0);
    vec3 skyCol(0.5, 0.7, 1.0);
    return (1.0 - t) * whiteCol + t * skyCol;
}

int main(int argc, char* argv[]) {
    const int imgWidth = 800;
    const int imgHeight = 400;

    // Establish sdl window
    sdltemplate::sdl("Raytracer", imgWidth, imgHeight);
    sdltemplate::loop();

    // Set coordinate system (UV)
    vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
    vec3 horizonatalVec(4.0, 0.0, 0.0);
    vec3 verticalVec(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    // Bottom to top (img is reversed), left to right
    for (int y = imgHeight - 1; y >= 0; y--) {
        for (int x = 0; x < imgWidth; x++) {
            //vec3 col(float(x) / (imgWidth - 1), float(y) / (imgHeight - 1), 0.25);
            float u = float(x) / float(imgWidth);
            float v = float(y) / float(imgHeight);

            ray rayDir(origin, lowerLeftCorner + u * horizonatalVec + v * verticalVec);
            vec3 col = colour(rayDir);


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