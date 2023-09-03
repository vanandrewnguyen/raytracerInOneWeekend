#include <iostream>
#include <memory>
#include <vector>
#include "sdltemplate.h"

// Objects
#include "Hitables/bvhNode.h"
#include "Hitables/hitableList.h"
#include "Hitables/sphere.h"
#include "Hitables/movingSphere.h"
#include "Hitables/box.h"
#include "Hitables/constantVolume.h"

// Manipulators
#include "translate.h"
#include "rotate.h"

// Camera
#include "camera.h"
#include "utility.h"

// Materials
#include "Materials/material.h"
#include "Materials/lambertian.h"
#include "Materials/metal.h"
#include "Materials/dielectric.h"
#include "Materials/diffuseLight.h"
#include "Materials/isotropic.h"

// Textures
#include "Textures/texture.h"
#include "Textures/texSolidColour.h"
#include "Textures/texChecker.h"
#include "Textures/texPerlin.h"
#include "Textures/texImage.h"

// Scene
#include "Scene/scene.h"
#include "Scene/sceneParser.h"

// Const
// Max depth is the maximum number of bounces a ray can have before destroying itself
float MAXFLOAT = 999.0;
int MAXDEPTH = 50;

vec3 scene(const Ray& r, vec3& bgCol, HitableList world, int depth, bool useSkyCol) {
    // Make new list of world items
    hitRecord rec;
    if (depth >= MAXDEPTH) {
        return vec3(0, 0, 0);
    }

    if (!(world.hit(r, 0.001, MAXFLOAT, rec))) {
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

void writeColourToScreen(int imgWidth, int imgHeight, Camera& cam, int x, int y, HitableList world, int sampleCount, vec3& bgCol, bool useSkyCol) {
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

void renderScene(Scene& parser, Camera& cam, HitableList& worldList, bool debugPrint = false) {
    srand((unsigned)time(NULL));

    // Establish SDL Window
    sdltemplate::sdl("Raytracer", parser.imageWidth, parser.imageHeight);
    sdltemplate::loop();

    for (int y = parser.imageHeight - 1; y >= 0; y--) {
        for (int x = 0; x < parser.imageWidth; x++) {
            // Output
            writeColourToScreen(parser.imageWidth, parser.imageHeight, cam, x, y, worldList, parser.sampleCount, parser.bgColour, parser.useSkyColour);
            // Debugging
            if (debugPrint) {
                std::cout << "Rendering pixel " << int(x + y) << std::endl;
            }
        }
    }

    if (debugPrint) {
        std::cout << "Done!" << std::endl;
    }

    // Keep window active
    while (sdltemplate::running) {
        sdltemplate::loop();
    }
}

int main(int argc, char* argv[]) {
    bool layout = false;

    if (layout) {
        SceneParser parser = SceneParser();
        std::string pathname = "sceneThreeSpheres.txt";
        Camera cam = parser.generateScene(pathname, true);
        HitableList worldList = parser.worldList;

        renderScene(parser, cam, worldList);
    } else {
        Scene scene = Scene();
        Camera cam = scene.getCornellBoxScene();
        HitableList worldList = scene.worldList;

        renderScene(scene, cam, worldList);
    }

    return 0;
}

/*
int renderSceneOld() {
    const int imgWidth = 200; // 800;
    const int imgHeight = 100; // 400;
    const int ns = 1; // 500;
    srand((unsigned)time(NULL));

    // Establish SDL Window
    sdltemplate::sdl("Raytracer", imgWidth, imgHeight);
    sdltemplate::loop();

    // Switch Scenes
    vec3 lookFrom, lookAt;
    float distFocus, aperture;
    HitableList world;
    int index = -1;
    vec3 bgCol = vec3(0, 0, 0);
    bool useSkyCol;

    switch (index) {
    case 0:
        lookFrom = vec3(3, 3, 2);
        lookAt = vec3(0, 0, -1);
        distFocus = (lookFrom - lookAt).length();
        aperture = 0.1;
        world = getBaseThreeSphereScene();
        useSkyCol = true;
        break;
    case 1:
        lookFrom = vec3(13, 2, 3);
        lookAt = vec3(0, 0, 0);
        distFocus = (lookFrom - lookAt).length();
        aperture = 0.2;
        world = getLargeRandomisedSphereScene();
        useSkyCol = true;
        break;
    case 2:
        lookFrom = vec3(6, 2, 4);
        lookAt = vec3(0, 0, -1);
        distFocus = (lookFrom - lookAt).length();
        aperture = 0.1;
        world = getMinimalLightsTestScene();
        useSkyCol = false;
        break;
    case 3:
        lookFrom = vec3(278, 278, -600);
        lookAt = vec3(0, 0, 0);
        distFocus = (lookFrom - lookAt).length();
        aperture = 40.0;
        world = getCornellBoxScene();
        useSkyCol = false;
        break;
    case 4:
        lookFrom = vec3(478, 278, -600);
        lookAt = vec3(0, 100, 0);
        distFocus = (lookFrom - lookAt).length();
        aperture = 40.0;
        world = getLargeMaterialShowcaseScene();
        useSkyCol = false;
        break;
    default:
        lookFrom = vec3(-3, 3, -2);
        lookAt = vec3(0, -0.1, -1);
        distFocus = (lookFrom - lookAt).length();
        aperture = 0.1;
        world = getMinimalTestScene();
        useSkyCol = true;
    }

    Camera cam(lookFrom, lookAt, vec3(0, 1, 0), 20, float(imgWidth) / float(imgHeight), aperture, distFocus, 0.0, 1.0);

    // Bottom to top (img is reversed), left to right
    for (int y = imgHeight - 1; y >= 0; y--) {
        for (int x = 0; x < imgWidth; x++) {
            // Output
            writeColourToScreen(imgWidth, imgHeight, cam, x, y, world, ns, bgCol, useSkyCol);
            // Debugging
            std::cout << "Rendered pixel " << int(x + y) << " of " << int(imgWidth * imgHeight) << "\n";
        }
    }
    std::cout << "Done!" << std::endl;

    // Keep window active
    while (sdltemplate::running) {
        sdltemplate::loop();
    }

    return 0;
}
*/