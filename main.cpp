#include <iostream>
#include "sdltemplate.h"

int main(int argc, char* argv[]) {
    const int imgWidth = 256;
    const int imgHeight = 256;

    // Establish sdl window
    sdltemplate::sdl("Raytracer", imgWidth, imgHeight);
    sdltemplate::loop();

    // Bottom to top (img is reversed), left to right
    for (int y = imgHeight - 1; y >= 0; y--) {
        for (int x = 0; x < imgWidth; x++) {
            float r = float(x) / (imgWidth - 1);
            float g = float(y) / (imgHeight - 1);
            float b = 0.25;

            // Normalize values
            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

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