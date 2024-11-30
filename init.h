#ifndef INIT_H
#define INIT_H


// STD Lib
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
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
#include "Hitables/flipFace.h"
#include "Hitables/triangle.h"

// Manipulators
#include "Transforms/translate.h"
#include "Transforms/rotate.h"

// Camera
#include "Render/camera.h"
#include "Utils/utility.h"

// Textures
#include "Textures/texture.h"
#include "Textures/texSolidColour.h"
#include "Textures/texChecker.h"
#include "Textures/texPerlin.h"
#include "Textures/texImage.h"
#include "Textures/texWorley.h"
#include "Textures/texNoise.h"
#include "Textures/texStripes.h"

// Materials
#include "Materials/material.h"
#include "Materials/lambertian.h"
#include "Materials/metal.h"
#include "Materials/dielectric.h"
#include "Materials/diffuseLight.h"
#include "Materials/isotropic.h"

// Normals
#include "Normals/normalBase.h"
#include "Normals/constant.h"
#include "Normals/rough.h"
#include "Normals/normalTexture.h"

// Scene
#include "Scene/scene.h"
#include "Scene/sceneParser.h"

// PDFs
#include "Utils/orthonormalBasis.h"
#include "Utils/spherePdf.h"
#include "Utils/cosinePdf.h"
#include "Utils/hitablePdf.h"
#include "Utils/mixturePdf.h"


#endif

