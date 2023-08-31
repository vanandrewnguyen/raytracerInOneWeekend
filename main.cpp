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

// Parser
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

// Get super basic scene with one scene
HitableList getMinimalTestScene() {
    HitableList worldList;
    worldList.append(std::make_shared<Sphere>(0.55, vec3(0.0, 0, -1), vec3(0, 0, 0), new MatLambertian(vec3(0.8, 0.3, 0.3), new TexImage("earthmap.jpg"))));
    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), new MatLambertian(vec3(0.8, 0.3, 0.3), new TexChecker(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0))));
    return worldList;
}

// Get identical scene from RayTracingInOneWeekend Book 1 final image
HitableList getLargeRandomisedSphereScene() {
    HitableList worldList;
    worldList.append(std::make_shared<Sphere>(1000, vec3(0, -1000, 0), vec3(0, 0, 0), new MatLambertian(vec3(0.5, 0.5, 0.5), new TexChecker(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0))));

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
                worldList.append(std::make_shared<MovingSphere>(centre, centre2, 0.0, 1.0, randRadius, vec3(0, 0, 0), new MatLambertian(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX))));
            } else if (randMat < 0.9) {
                worldList.append(std::make_shared<Sphere>(randRadius, centre, vec3(0, 0, 0), new MatMetal(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX), ((float)rand() / RAND_MAX) * 0.5)));
            } else {
                worldList.append(std::make_shared<Sphere>(randRadius, centre, vec3(0, 0, 0), new MatDielectric(((float)rand() / RAND_MAX) + 0.5)));
            }

        }
    }
    worldList.append(std::make_shared<Sphere>(1, vec3(0,1,0), vec3(0,0,0), new MatDielectric(1.5)));
    worldList.append(std::make_shared<Sphere>(1, vec3(-4, 1, 0), vec3(0, 0, 0), new MatLambertian(vec3(0.4, 0.2, 0.1))));
    worldList.append(std::make_shared<Sphere>(1, vec3(4, 1, 0), vec3(0, 0, 0), new MatMetal(vec3(0.7, 0.6, 0.5), 0.5)));

    return worldList;
}

// Get scene with three test spheres of different materials
HitableList getBaseThreeSphereScene() {
    HitableList worldList;
    worldList.append(std::make_shared<Sphere>(0.55, vec3(1.0, 0, -1), vec3(1, 0, 0), new MatLambertian(vec3(0.9, 0.8, 0.9))));
    worldList.append(std::make_shared<Sphere>(0.55, vec3(-1.0, 0, -1), vec3(1, 0, 0), new MatMetal(vec3(1.0, 1.0, 1.0), 0.5)));
    worldList.append(std::make_shared<Sphere>(0.55, vec3(0.0, 0, -1), vec3(1, 0, 0), new MatDielectric(1.33)));
    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), new MatLambertian(vec3(0.8, 0.3, 0.3), new TexChecker(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0))));
    return worldList;
}

// Get scene with minimal lighting and two lights, and one sphere with a marble texture
HitableList getMinimalLightsTestScene() {
    HitableList worldList;
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, -0.5), vec3(1, 0, 0), new MatLambertian(vec3(0.9, 0.8, 0.9), new TexPerlin(2.0, 4, vec3(0,0,0), vec3(0.7,0.6,0.5)))));
    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), new MatLambertian(vec3(0.8, 0.3, 0.3), new TexChecker(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0))));
    worldList.append(std::make_shared<Sphere>(0.4, vec3(-0.4, 1.1, 0.6), vec3(1, 0, 0), new DiffuseLight(new TexSolidColour(vec3(4, 4, 4)))));
    worldList.append(std::make_shared<XYRect>(0.15, 1.0, -0.2, 0.5, -1.5, new DiffuseLight(new TexSolidColour(vec3(4, 4, 4)))));
    return worldList;
}

// Get scene identical of the Cornell Box layout
HitableList getCornellBoxScene() {
    HitableList worldList;
    
    auto redMat = new MatLambertian(vec3(0.8, 0.1, 0.1));
    auto greenMat = new MatLambertian(vec3(0.2, 0.8, 0.2));
    auto whiteMat = new MatLambertian(vec3(0.8, 0.8, 0.8));
    auto lightMat = new DiffuseLight(new TexSolidColour(vec3(4, 4, 4)));

    worldList.append(std::make_shared<YZRect>(0, 555, 0, 555, 555, greenMat));
    worldList.append(std::make_shared<YZRect>(0, 555, 0, 555, 0, redMat));
    worldList.append(std::make_shared<XZRect>(213-96, 343+94, 227-64, 332+64, 554, lightMat));
    worldList.append(std::make_shared<XZRect>(0, 555, 0, 555, 0, whiteMat));
    worldList.append(std::make_shared<XZRect>(0, 555, 0, 555, 555, whiteMat));
    worldList.append(std::make_shared<XYRect>(0, 555, 0, 555, 555, whiteMat));

    Hitable* box1 = new Box(vec3(130, 0, 65), vec3(295, 165, 230), whiteMat);
    Hitable* box2 = new Box(vec3(265, 0, 295), vec3(430, 330, 460), whiteMat);
    worldList.append(std::make_shared<ConstantVolume>(box1, 0.01, new TexSolidColour(vec3(1,1,1))));
    worldList.append(std::make_shared<ConstantVolume>(box2, 0.01, new TexSolidColour(vec3(0, 0, 0))));

    return worldList;
}

// Get identical scene from RayTracingInOneWeekend Book 2 final image
HitableList getLargeMaterialShowcaseScene() {
    HitableList worldList;
    int len = 0;

    // Random box floor
    Material* groundMat = new MatLambertian(vec3(0.48, 0.83, 0.53));
    const int boxesPerSide = 10;
    for (int i = 0; i < boxesPerSide; i++) {
        for (int j = 0; j < boxesPerSide; j++) {
            float size = 100.0;
            float x0 = -size * (boxesPerSide / 2) + i * size;
            float z0 = -size * (boxesPerSide / 2) + j * size;
            float y0 = 0.0;
            float x1 = x0 + size;
            float y1 = Utility::randomDouble(1, 101);
            float z1 = z0 + size;
            worldList.append(std::make_shared<Box>(vec3(x0, y0, z0), vec3(x1, y1, z1), groundMat));
        }
    }

    // Light
    worldList.append(std::make_shared<XYRect>(123, 423, 147, 412, 554, new DiffuseLight(new TexSolidColour(4, 4, 4))));
    worldList.append(std::make_shared<Sphere>(80, vec3(60, 280, 0), vec3(1, 0, 0), new DiffuseLight(new TexSolidColour(vec3(4, 4, 4)))));

    // Volumetric Fog
    auto volumeBoundaryFog = new Sphere(5000, vec3(60, 280, 0), vec3(0, 0, 0), new MatDielectric(1.5));
    worldList.append(std::make_shared<ConstantVolume>(volumeBoundaryFog, 0.0001, new TexSolidColour(1, 1, 1)));

    // 5 Spheres
    worldList.append(std::make_shared<Sphere>(30, vec3(0, 130, 0), vec3(0, 0, 0), new MatDielectric(1.5)));
    worldList.append(std::make_shared<Sphere>(30, vec3(-80, 150, 0), vec3(0, 0, 0), new MatDielectric(1.5)));
    worldList.append(std::make_shared<ConstantVolume>(new Sphere(30, vec3(-80, 150, 0), vec3(0, 0, 0), new MatDielectric(1.5)), 0.2, new TexSolidColour(0.2, 0.4, 0.7)));
    worldList.append(std::make_shared<Sphere>(30, vec3(80, 150, 0), vec3(0, 0, 0), new MatMetal(vec3(0.7, 0.6, 0.5), 0.5)));
    worldList.append(std::make_shared<Sphere>(30, vec3(-160, 130, 0), vec3(0, 0, 0), new MatLambertian(vec3(0.9, 0.8, 0.9), new TexPerlin(0.04, 4, vec3(0, 0, 0), vec3(0.7, 0.6, 0.5)))));
    worldList.append(std::make_shared<ConstantVolume>(new Sphere(30, vec3(160, 130, 0), vec3(0, 0, 0), new MatDielectric(1.5)), 0.01, new TexSolidColour(0.8, 0.3, 0.3)));

    return worldList;
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

int main(int argc, char* argv[]) {
    //const int imgWidth = 200; // 800;
    //const int imgHeight = 100; // 400;
    //const int ns = 1; // 500;
    //srand((unsigned)time(NULL));

    //// Establish SDL Window
    //sdltemplate::sdl("Raytracer", imgWidth, imgHeight);
    //sdltemplate::loop();

    //// Switch Scenes
    //vec3 lookFrom, lookAt;
    //float distFocus, aperture;
    //HitableList world;
    //int index = 0;
    //vec3 bgCol = vec3(0,0,0);
    //bool useSkyCol;

    //switch (index) {
    //    case 0:
    //        lookFrom = vec3(3, 3, 2);
    //        lookAt = vec3(0, 0, -1);
    //        distFocus = (lookFrom - lookAt).length();
    //        aperture = 0.1;
    //        world = getBaseThreeSphereScene();
    //        useSkyCol = true;
    //    break;
    //    case 1:
    //        lookFrom = vec3(13, 2, 3);
    //        lookAt = vec3(0, 0, 0);
    //        distFocus = (lookFrom - lookAt).length();
    //        aperture = 0.2;
    //        world = getLargeRandomisedSphereScene();
    //        useSkyCol = true;
    //    break;
    //    case 2:
    //        lookFrom = vec3(6, 2, 4);
    //        lookAt = vec3(0, 0, -1);
    //        distFocus = (lookFrom - lookAt).length();
    //        aperture = 0.1;
    //        world = getMinimalLightsTestScene();
    //        useSkyCol = false;
    //    break;
    //    case 3:
    //        lookFrom = vec3(278, 278, -600);
    //        lookAt = vec3(0, 0, 0);
    //        distFocus = (lookFrom - lookAt).length();
    //        aperture = 40.0;
    //         world = getCornellBoxScene();
    //        useSkyCol = false;
    //    break;
    //    case 4:
    //        lookFrom = vec3(478, 278, -600);
    //        lookAt = vec3(0, 100, 0);
    //        distFocus = (lookFrom - lookAt).length();
    //        aperture = 40.0;
    //         world = getLargeMaterialShowcaseScene();
    //        useSkyCol = false;
    //    break;
    //    default:
    //        lookFrom = vec3(-3, 3, -2); 
    //        lookAt = vec3(0, -0.1, -1);
    //        distFocus = (lookFrom - lookAt).length();
    //        aperture = 0.1;
    //        world = getMinimalTestScene();
    //        useSkyCol = true;
    //}

    //Camera cam(lookFrom, lookAt, vec3(0, 1, 0), 20, float(imgWidth) / float(imgHeight), aperture, distFocus, 0.0, 1.0);

    //// Bottom to top (img is reversed), left to right
    //for (int y = imgHeight - 1; y >= 0; y--) {
    //    for (int x = 0; x < imgWidth; x++) {
    //        // Output
    //        writeColourToScreen(imgWidth, imgHeight, cam, x, y, world, ns, bgCol, useSkyCol);
    //        // Debugging
    //        std::cout << "Rendered pixel " << int(x + y) << " of " << int(imgWidth * imgHeight) << "\n";

    //    }
    //}
    //std::cout << "Done!" << std::endl;

    //// Keep window active
    //while (sdltemplate::running) {
    //    sdltemplate::loop();
    //}

    SceneParser parser = SceneParser();
    std::string pathname = "sceneTest.txt";
    HitableList worldList = parser.getWorldList(pathname);

    return 0;
}