#include <iostream>
#include "sdltemplate.h"
#include "hitableList.h"
#include "sphere.h"
#include "camera.h"

#include "utility.h"

#include "Materials/material.h"
#include "Materials/lambertian.h"
#include "Materials/metal.h"
#include "Materials/dielectric.h"

float MAXFLOAT = 999.0;
int MAXDEPTH = 50;

vec3 getSkyColour(const Ray& r) {
    vec3 unitDir = unitVector(r.getDirection());
    // Shift from -1->1 to 0->1
    float t = 0.5 * (unitDir.getY() + 1.0);

    // Interpolation
    vec3 whiteCol(1.0, 1.0, 1.0);
    vec3 skyCol(0.5, 0.7, 1.0);
    return (1.0 - t) * whiteCol + t * skyCol;
}

vec3 scene(const Ray& r, Hitable *world, int depth) {
    // Make new list of world items
    hitRecord rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        Ray scatteredRay;
        vec3 attenuation;
        // Push off the surface normal a little bit (no collision error)
        // Recursion for shadows
        if (depth < MAXDEPTH && rec.matPtr->scatter(r, rec, attenuation, scatteredRay)) {
            return attenuation * scene(scatteredRay, world, depth + 1);
        } else {
            return vec3(0, 0, 0);
        }
    }
    
    // If no collision, return sky colour
    return getSkyColour(r);
}

Hitable* getRandomScene() {
    int n = 500;
    Hitable** list = new Hitable * [n + 1];
    list[0] = new Sphere(1000, vec3(0, -1000, 0), vec3(0,0,0), new MatLambertian(vec3(0.5, 0.5, 0.5)));
    
    // Generate random spheres
    int size = 11;
    int len = 1;
    for (int a = -size; a < size; a++) {
        for (int b = -size; b < size; b++) {
            float randMat = ((float)rand() / RAND_MAX);
            float randRadius = 0.1 + 0.1 * ((float)rand() / RAND_MAX);
            vec3 centre(a + 0.9 * (float)rand() / RAND_MAX, randRadius, b + 0.9 * (float)rand() / RAND_MAX);
            if (randMat < 0.33) {
                list[len++] = new Sphere(randRadius, centre, vec3(0,0,0), new MatLambertian(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX)));
            } else if (randMat < 0.66) {
                list[len++] = new Sphere(randRadius, centre, vec3(0, 0, 0), new MatMetal(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX), ((float)rand() / RAND_MAX) * 0.5));
            } else {
                list[len++] = new Sphere(randRadius, centre, vec3(0, 0, 0), new MatDielectric(((float)rand() / RAND_MAX) + 0.5));
            }

        }
    }
    list[len++] = new Sphere(1, vec3(0,1,0), vec3(0,0,0), new MatDielectric(1.5));
    list[len++] = new Sphere(1, vec3(-4, 1, 0), vec3(0, 0, 0), new MatLambertian(vec3(0.4, 0.2, 0.1)));
    list[len++] = new Sphere(1, vec3(4, 1, 0), vec3(0, 0, 0), new MatMetal(vec3(0.7, 0.6, 0.5), 0.5));

    return new HitableList(list, len);
}

int main(int argc, char* argv[]) {
    const int imgWidth = 800;
    const int imgHeight = 400;
    const int ns = 1; //9
    srand((unsigned)time(NULL));

    // Establish SDL Window
    sdltemplate::sdl("Raytracer", imgWidth, imgHeight);
    sdltemplate::loop();

    // Make camera
    vec3 lookFrom(13, 2, 3); //(3, 3, 2); // 0,0,0.5
    vec3 lookAt(0,0,0); //(0, 0, -1);
    float distFocus = (lookFrom - lookAt).length();
    float aperture = 0.2; // 1.5, fov should be 90 not 20 (20 is zoomed in)
    Camera cam(lookFrom, lookAt, vec3(0,1,0), 20, float(imgWidth)/float(imgHeight), aperture, distFocus);

    // Establish list of world items (can push into seperate function)
    /*
    Hitable* worldList[4];
    worldList[0] = new Sphere(0.55, vec3(1.0, 0, -1), vec3(1, 0, 0), new MatLambertian(vec3(0.9, 0.8, 0.9)));
    worldList[1] = new Sphere(0.55, vec3(-1.0, 0, -1), vec3(1, 0, 0), new MatMetal(vec3(1.0, 1.0, 1.0), 0.5));
    worldList[2] = new Sphere(0.55, vec3(0.0, 0, -1), vec3(1, 0, 0), new MatDielectric(1.33));
    worldList[3] = new Sphere(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), new MatLambertian(vec3(0.8, 0.3, 0.3)));
    Hitable* world = new HitableList(worldList, 4);
    */
    Hitable* world = getRandomScene();

    // Bottom to top (img is reversed), left to right
    for (int y = imgHeight - 1; y >= 0; y--) {
        for (int x = 0; x < imgWidth; x++) {
            // Set UV's
            // We can offset randomly to anti alias cheaply, moving the cam
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = float(x + (float)rand() / RAND_MAX) / float(imgWidth);
                float v = float(y + (float)rand() / RAND_MAX) / float(imgHeight);

                // Get sky colour
                Ray rayDir = cam.getRay(u, v);
                vec3 pos = rayDir.getPointParam(2.0);
                col += scene(rayDir, world, 0);
            }
            // Offset
            col /= float(ns);

            // Colour correction
            col = vec3(sqrt(col.getX()), sqrt(col.getY()), sqrt(col.getZ()));

            // Normalize values
            int ir = static_cast<int>(255.999 * col.getX());
            int ig = static_cast<int>(255.999 * col.getY());
            int ib = static_cast<int>(255.999 * col.getZ());

            // Output
            sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
            sdltemplate::drawPoint(x, imgHeight-y);
            //std::cout << int(x+y) << int(imgWidth * imgHeight) << "\n";
        }
    }
    
    // Keep window active
    while (sdltemplate::running) {
        sdltemplate::loop();
    }

    return 0;
}