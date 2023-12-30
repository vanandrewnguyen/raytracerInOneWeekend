#pragma once

// STD Lib
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <utility>

// Rendering
#include "sdltemplate.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
#include "Textures/texWorley.h"
#include "Textures/texNoise.h"
#include "Textures/texStripes.h"

// Normals
#include "Normals/normalBase.h"
#include "Normals/constant.h"
#include "Normals/rough.h"

// Scene
#include "Scene/scene.h"
#include "Scene/sceneParser.h"

// Const
// Max depth is the maximum number of bounces a ray can have before destroying itself
float MAXFLOAT = 999.0;
int MAXDEPTH = 50;
int imageWidth = 200;
int imageHeight = 100;
int sampleCount = 1;

namespace render {
    vec3 scene(const Ray& r, vec3& bgCol, HitableList world, int depth, int useSkyCol) {
        // Make new list of world items
        hitRecord rec;
        if (depth >= MAXDEPTH) {
            return vec3(0, 0, 0);
        }

        if (!(world.hit(r, 0.001, MAXFLOAT, rec))) {
            // If no collision, return sky colour
            switch (useSkyCol) {
            case 0:
                return bgCol;
                break;
            case 1:
                return Utility::getFakeScatteringSkyGradient(r);
                break;
            case 2:
                return Utility::getSimpleSkyGradient(r);
                break;
            case 3:
                return Utility::getNightSkyGradient(r);
                break;
            default:
                return bgCol;
            }
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

    void writeColourToScreen(int imgWidth, int imgHeight, Camera& cam, int x, int y, HitableList world, int sampleCount, vec3& bgCol, int useSkyCol, std::vector<std::tuple<int, int, int>>& pixelData) {
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
        int ir = Utility::clamp(static_cast<int>(255.999 * col.getX()), 0, 255);
        int ig = Utility::clamp(static_cast<int>(255.999 * col.getY()), 0, 255);
        int ib = Utility::clamp(static_cast<int>(255.999 * col.getZ()), 0, 255);

        // Write in real time
        if (x % imgWidth == 1) sdltemplate::loop();

        // Output
        sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
        sdltemplate::drawPoint(x, imgHeight - y);

        // Write to pixel data
        pixelData.emplace_back(ir, ig, ib);
    }

    void renderScene(Scene& parser, Camera& cam, HitableList& worldList, std::vector<std::tuple<int, int, int>>& pixelData, bool debugPrint = false) {
        srand((unsigned)time(NULL));

        // Establish SDL Window
        sdltemplate::sdl("Raytracer", parser.imageWidth, parser.imageHeight);
        sdltemplate::loop();
        std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

        for (int y = parser.imageHeight - 1; y >= 0; y--) {
            for (int x = 0; x < parser.imageWidth; x++) {
                // Output
                writeColourToScreen(parser.imageWidth, parser.imageHeight, cam, x, y, worldList, parser.sampleCount, parser.bgColour, parser.useSkyColour, pixelData);
                // Debugging
                if (debugPrint) {
                    std::cout << "Rendering pixel " << int(x + y) << std::endl;
                }
            }
        }

        // Print time taken
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        Utility::printTimeTaken(startTime, endTime);

        if (debugPrint) {
            std::cout << "Done!" << std::endl;
        }

        // Keep window active
        while (sdltemplate::running) {
            // Handle events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    sdltemplate::running = false;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        sdltemplate::running = false;
                    }
                }
            }

            sdltemplate::loop();
        }

        SDL_Quit();
    }

    void saveToBMP(const char* filename, int width, int height, const std::vector<std::tuple<int, int, int>>& pixels) {
        std::vector<uint8_t> bmpData; // Create a vector to store the BMP data

        // Convert the RGB pixel data to BMP format (BGR)
        for (const auto& pixel : pixels) {
            uint8_t b = static_cast<uint8_t>(std::get<0>(pixel));
            uint8_t g = static_cast<uint8_t>(std::get<1>(pixel));
            uint8_t r = static_cast<uint8_t>(std::get<2>(pixel));

            bmpData.push_back(b);
            bmpData.push_back(g);
            bmpData.push_back(r);
        }

        // Use stb_image_write to save the BMP file
        if (!stbi_write_bmp(filename, width, height, 3, bmpData.data())) {
            std::cerr << "Failed to save the BMP image." << std::endl;
        }
    }
}

namespace userInput {
    void printPremadeScenes() {
        std::cout << "List of premade scenes:" << std::endl;
        for (const auto& pair : Scene::sceneMapping) {
            std::cout << pair.first << ". " << pair.second << std::endl;
        }
    }
    
    void printSceneFiles(std::string path) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    std::cout << "Found .txt file: " << entry.path().filename() << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    std::pair<bool, std::string> getUserCmd() {
        int choice;
        std::string userInput;
        std::string w, h, ns;

        while (true) {
            std::cout << "Please choose an option:" << std::endl;
            std::cout << "1. Render a premade scene" << std::endl;
            std::cout << "2. Import a scene file" << std::endl;
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            if (choice == 1) {
                std::cout << "Type 'help' for a list of scenes or enter the scene number: ";
                std::cin >> userInput;

                if (userInput == "help") {
                    printPremadeScenes();
                } else {
                    if (Utility::isConvertibleToInt(userInput)) {
                        std::cout << "Image Width: ";
                        std::cin >> w;
                        std::cout << "Image Height: ";
                        std::cin >> h;
                        std::cout << "Sample Count: ";
                        std::cin >> ns;
                        imageWidth = atoi(w.c_str());
                        imageHeight = atoi(h.c_str());
                        sampleCount = atoi(ns.c_str());

                        std::cout << "Selected scene number: " << userInput << std::endl;
                        return std::pair<bool, std::string>(false, std::string(userInput));
                    } else {
                        break;
                    }
                }
            } else if (choice == 2) {
                std::cout << "Type 'help' for a list of present files or enter the file name: ";
                std::cin >> userInput;

                if (userInput == "help") {
                    printSceneFiles(std::string("Scene"));
                } else {
                    return std::pair<bool, std::string>(true, std::string(userInput));
                }
            } else {
                std::cout << "Invalid choice. Please try again.\n";
            }

            std::cout << std::endl;
        }

        // Default
        return std::pair<bool, std::string>(true, std::string("sceneSingleSphere.txt"));
    }

    void GetUserWriteToFile(std::vector<std::tuple<int, int, int>>& pixelData, int imageWidth, int imageHeight) {
        std::cout << "Would you like to write the image to a file? (Y/N) ";
        std::string userInput;
        std::cin >> userInput;

        if ((userInput == "y") || (userInput == "Y")) {
            std::cout << "Enter file name: ";
            std::string fileName;
            std::cin >> fileName;

            std::string tmp = std::string("./Outputs/" + fileName + ".BMP");
            const char* name = tmp.c_str();

            render::saveToBMP(name, imageWidth, imageHeight, pixelData);
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    while (true) {
        std::pair<bool, std::string> userCmd = userInput::getUserCmd();
        std::vector<std::tuple<int, int, int>> pixelData;
        if (userCmd.first) {
            SceneParser parser = SceneParser();
            Camera cam = parser.generateScene(userCmd.second, true);
            HitableList worldList = parser.worldList;
            render::renderScene(parser, cam, worldList, pixelData);
        } else {
            Scene scene = Scene();
            Camera cam = scene.generateSceneFromMapping(atoi(userCmd.second.c_str()), imageWidth, imageHeight, sampleCount);
            HitableList worldList = scene.worldList;
            render::renderScene(scene, cam, worldList, pixelData);
        }
        userInput::GetUserWriteToFile(pixelData, imageWidth, imageHeight);
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