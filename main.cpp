#include <iostream>
#include "sdltemplate.h"

// Objects
#include "Hitables/hitableList.h"
#include "Hitables/sphere.h"
#include "Hitables/movingSphere.h"
//#include "Hitables/xyRect.h"
//#include "Hitables/yzRect.h"
//#include "Hitables/xzRect.h"
#include "Hitables/box.h"

#include "camera.h"
#include "utility.h"

// Materials
#include "Materials/material.h"
#include "Materials/lambertian.h"
#include "Materials/metal.h"
#include "Materials/dielectric.h"
#include "Materials/diffuseLight.h"

// Textures
#include "Textures/texture.h"
#include "Textures/texSolidColour.h"
#include "Textures/texChecker.h"
#include "Textures/texPerlin.h"
#include "Textures/texImage.h"

// Const
float MAXFLOAT = 999.0;
int MAXDEPTH = 50;

vec3 scene(const Ray& r, vec3& bgCol, Hitable *world, int depth, bool useSkyCol) {
    // Make new list of world items
    hitRecord rec;
    if (depth >= MAXDEPTH) {
        return vec3(0, 0, 0);
    }

    if (!(world->hit(r, 0.001, MAXFLOAT, rec))) {
        // If no collision, return sky colour
        return (useSkyCol) ? Utility::getSkyColour(r) : bgCol;
    }
    
    // Else, do our recursive calls
    Ray scatteredRay;
    vec3 attenuation;
    vec3 emitted = rec.matPtr->emitted(rec.u, rec.v, rec.pos);
    // Push off the surface normal a little bit (no collision error)
    if (!(rec.matPtr->scatter(r, rec, attenuation, scatteredRay))) {
        return emitted;
    }

    return emitted + attenuation * scene(scatteredRay, bgCol, world, depth + 1, useSkyCol);
    
    /*
    if (depth < MAXDEPTH && rec.matPtr->scatter(r, rec, attenuation, scatteredRay)) {
        return attenuation * scene(scatteredRay, world, depth + 1);
    } else {
        return vec3(0, 0, 0);
    }*/
}

Hitable* getRandomScene() {
    int n = 500;
    Hitable** list = new Hitable * [n + 1];
    //list[0] = new Sphere(1000, vec3(0, -1000, 0), vec3(0,0,0), new MatLambertian(vec3(0.5, 0.5, 0.5)));
    list[0] = new Sphere(1000, vec3(0, -1000, 0), vec3(0, 0, 0), new MatLambertian(vec3(0.5, 0.5, 0.5), new TexChecker(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0)));

    // Generate random spheres
    int size = 11;
    int len = 1;
    for (int a = -size; a < size; a++) {
        for (int b = -size; b < size; b++) {
            float randMat = ((float)rand() / RAND_MAX);
            float randRadius = 0.1 + 0.1 * ((float)rand() / RAND_MAX);
            vec3 centre(a + 0.9 * (float)rand() / RAND_MAX, randRadius, b + 0.9 * (float)rand() / RAND_MAX);
            if (randMat < 0.8) {
                // Lambertian spheres have a random centre
                vec3 centre2 = centre + vec3(0, Utility::randomDouble(0, 0.4), 0);
                //list[len++] = new Sphere(randRadius, centre, vec3(0,0,0), new MatLambertian(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX)));
                list[len++] = new MovingSphere(centre, centre2, 0.0, 1.0, randRadius, vec3(0, 0, 0), new MatLambertian(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX)));
            } else if (randMat < 0.9) {
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

Hitable* getBaseThreeSphereScene() {
    Hitable** worldList = new Hitable * [4];
    worldList[0] = new Sphere(0.55, vec3(1.0, 0, -1), vec3(1, 0, 0), new MatLambertian(vec3(0.9, 0.8, 0.9)));
    worldList[1] = new Sphere(0.55, vec3(-1.0, 0, -1), vec3(1, 0, 0), new MatMetal(vec3(1.0, 1.0, 1.0), 0.5));
    worldList[2] = new Sphere(0.55, vec3(0.0, 0, -1), vec3(1, 0, 0), new MatDielectric(1.33));
    worldList[3] = new Sphere(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), new MatLambertian(vec3(0.8, 0.3, 0.3), new TexChecker(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0)));
    return new HitableList(worldList, 4);
}

Hitable* getMinimalOneSphereScene() {
    Hitable** worldList = new Hitable * [4];
    worldList[0] = new Sphere(0.5, vec3(0, 0, -0.5), vec3(1, 0, 0), new MatLambertian(vec3(0.9, 0.8, 0.9), new TexPerlin(2.0, 4, vec3(0,0,0), vec3(0.7,0.6,0.5))));
    worldList[1] = new Sphere(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), new MatLambertian(vec3(0.8, 0.3, 0.3), new TexChecker(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0)));
    worldList[2] = new Sphere(0.4, vec3(-0.4, 1.1, 0.6), vec3(1, 0, 0), new DiffuseLight(new TexSolidColour(vec3(4, 4, 4))));
    worldList[3] = new XYRect(0.15, 1.0, -0.2, 0.5, -1.5, new DiffuseLight(new TexSolidColour(vec3(4, 4, 4))));
    return new HitableList(worldList, 4);
}

Hitable* getCornellBoxScene() {
    Hitable** worldList = new Hitable * [8];
    
    auto redMat = new MatLambertian(vec3(0.8, 0.1, 0.1));
    auto greenMat = new MatLambertian(vec3(0.2, 0.8, 0.2));
    auto whiteMat = new MatLambertian(vec3(0.8, 0.8, 0.8));
    auto lightMat = new DiffuseLight(new TexSolidColour(vec3(4, 4, 4)));

    worldList[0] = new YZRect(0, 555, 0, 555, 555, greenMat);
    worldList[1] = new YZRect(0, 555, 0, 555, 0, redMat);
    worldList[2] = new XZRect(213-96, 343+94, 227-64, 332+64, 554, lightMat);
    worldList[3] = new XZRect(0, 555, 0, 555, 0, whiteMat);
    worldList[4] = new XZRect(0, 555, 0, 555, 555, whiteMat);
    worldList[5] = new XYRect(0, 555, 0, 555, 555, whiteMat);

    worldList[6] = new Box(vec3(130, 0, 65), vec3(295, 165, 230), whiteMat);
    worldList[7] = new Box(vec3(265, 0, 295), vec3(430, 330, 460), whiteMat);

    return new HitableList(worldList, 8);
}

void writeColourToScreen(int imgWidth, int imgHeight, Camera& cam, int x, int y, Hitable* world, int sampleCount, vec3& bgCol, bool useSkyCol) {
    // Set UV's
    // We can offset randomly to anti alias cheaply, moving the cam
    vec3 col(0, 0, 0);
    for (int s = 0; s < sampleCount; s++) {
        float u = float(x + (float)rand() / RAND_MAX) / float(imgWidth);
        float v = float(y + (float)rand() / RAND_MAX) / float(imgHeight);

        // Get sky colour
        Ray rayDir = cam.getRay(u, v);
        vec3 pos = rayDir.getPointParam(2.0);
        col += scene(rayDir, bgCol, world, 0, useSkyCol);
    }
    // Divide by sample count
    col /= float(sampleCount);

    // Colour gamma correction
    col = vec3(sqrt(col.getX()), sqrt(col.getY()), sqrt(col.getZ()));

    // Normalize values
    int ir = static_cast<int>(255.999 * col.getX());
    int ig = static_cast<int>(255.999 * col.getY());
    int ib = static_cast<int>(255.999 * col.getZ());

    // Write in real time
    if (x % imgWidth == 1) sdltemplate::loop();

    // Output
    sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
    sdltemplate::drawPoint(x, imgHeight - y);
}

int main(int argc, char* argv[]) {
    const int imgWidth = 800;
    const int imgHeight = 400;
    const int ns = 200; //9
    srand((unsigned)time(NULL));

    // Establish SDL Window
    sdltemplate::sdl("Raytracer", imgWidth, imgHeight);
    sdltemplate::loop();

    // Switch Scenes
    vec3 lookFrom, lookAt;
    float distFocus, aperture;
    Hitable* world;
    int index = 3;
    vec3 bgCol = vec3(0,0,0);
    bool useSkyCol = true;

    switch (index) {
        case 1:
            lookFrom = vec3(13, 2, 3);
            lookAt = vec3(0, 0, 0);
            distFocus = (lookFrom - lookAt).length();
            aperture = 0.2;
            world = getRandomScene();
        break;
        case 2:
            lookFrom = vec3(6, 2, 4);
            lookAt = vec3(0, 0, -1);
            distFocus = (lookFrom - lookAt).length();
            aperture = 0.1;
            world = getMinimalOneSphereScene();
            useSkyCol = false;
        break;
        case 3:
            lookFrom = vec3(278, 278, -1800);
            lookAt = vec3(278, 278, 0);
            distFocus = (lookFrom - lookAt).length();
            aperture = 2.0;
            world = getCornellBoxScene();
            useSkyCol = false;
        break;
        default:
            lookFrom = vec3(3, 3, 2); 
            lookAt = vec3(0, 0, -1);
            distFocus = (lookFrom - lookAt).length();
            aperture = 0.1;
            world = getBaseThreeSphereScene();
    }
    Camera cam(lookFrom, lookAt, vec3(0, 1, 0), 20, float(imgWidth) / float(imgHeight), aperture, distFocus, 0.0, 1.0);

    // Bottom to top (img is reversed), left to right
    for (int y = imgHeight - 1; y >= 0; y--) {
        for (int x = 0; x < imgWidth; x++) {
            // Output
            writeColourToScreen(imgWidth, imgHeight, cam, x, y, world, ns, bgCol, useSkyCol);
            // Debugging
            //std::cout << int(x+y) << int(imgWidth * imgHeight) << "\n";
        }
    }
    
    // Keep window active
    while (sdltemplate::running) {
        sdltemplate::loop();
    }

    return 0;
}