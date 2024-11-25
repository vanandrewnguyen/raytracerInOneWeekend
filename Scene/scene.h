#ifndef SCENE_H
#define SCENE_H

#include "../Hitables/hitable.h"
#include "../Hitables/hitableList.h"
#include <map>

class Scene {
public:
    Scene();
	Scene(int _imageWidth, int _imageHeight, int _sampleCount, 
          vec3 _lookFrom, vec3 _lookAt,
          float _viewFOV, float _aperture, float _focusDist, float time0, float time1,
          vec3 _bgColour, int _useSkyColour);

    // Hard-coded scenes
    Camera getCornellBoxScene();
    Camera getBook1Scene();
    Camera getBook2Scene();
    Camera getTextureMaterialShowcase();
    Camera getDebugScene();
    Camera generateSceneFromMapping(int index, int _imageWidth, int _imageHeight, int _sampleCount);

public:
	HitableList worldList;
    int imageWidth, imageHeight, sampleCount;
    vec3 lookFrom, lookAt, bgColour;
    float viewFOV, aperture, focusDist, timeStart, timeEnd;
    int useSkyColour;

    static inline std::map<int, std::string> sceneMapping = { {1, std::string("Cornell Box")},
                                                              {2, std::string("Infinite Spheres on Checkerboard")},
                                                              {3, std::string("Spheres and Cubes in Shadow")},
                                                              {4, std::string("Material/Texture Showcase")},
                                                              {5, std::string("Debug Scene")} };
};

Scene::Scene() {}

Scene::Scene(int _imageWidth, int _imageHeight, int _sampleCount,
    vec3 _lookFrom, vec3 _lookAt,
    float _viewFOV, float _aperture, float _focusDist, float time0, float time1,
    vec3 _bgColour, int _useSkyColour) {
    imageWidth = _imageWidth;
    imageHeight = _imageHeight;
    sampleCount = _sampleCount;
    lookFrom = _lookFrom;
    lookAt = _lookAt;
    bgColour = _bgColour;
    useSkyColour = _useSkyColour;
    viewFOV = _viewFOV;
    aperture = _aperture;
    focusDist = _focusDist;
    timeStart = time0;
    timeEnd = time1;
}

Camera Scene::generateSceneFromMapping(int index, int _imageWidth, int _imageHeight, int _sampleCount) {
    imageWidth = _imageWidth;
    imageHeight = _imageHeight;
    sampleCount = _sampleCount;

    if (index == 1) return getCornellBoxScene();
    if (index == 2) return getBook1Scene();
    if (index == 3) return getBook2Scene();
    if (index == 4) return getTextureMaterialShowcase();
    if (index == 5) return getDebugScene();

    return getCornellBoxScene();
}

// Get scene identical of the Cornell Box layout
Camera Scene::getCornellBoxScene() {
    // Set proper camera values
    lookFrom = vec3(278, 278, -1800);
    lookAt = vec3(278, 278, 0);
    bgColour = vec3(0, 0, 0);
    useSkyColour = 1; // 0
    viewFOV = 20;
    aperture = 40.0;
    focusDist = (lookFrom - lookAt).length();
    timeStart = 0;
    timeEnd = 1;

    // Render
    std::shared_ptr<Material> redMat = std::make_shared<MatLambertian>(vec3(0.8, 0.1, 0.1));
    std::shared_ptr<Material> greenMat = std::make_shared<MatLambertian>(vec3(0.2, 0.8, 0.2));
    std::shared_ptr<Material> whiteMat = std::make_shared<MatLambertian>(vec3(0.8, 0.8, 0.8));
    std::shared_ptr<Material> lightMat = std::make_shared<DiffuseLight>(std::make_shared<TexSolidColour>(vec3(4, 4, 4)));

    worldList.append(std::make_shared<YZRect>(0, 555, 0, 555, 555, greenMat));
    worldList.append(std::make_shared<YZRect>(0, 555, 0, 555, 0, redMat));
    worldList.append(std::make_shared<XZRect>(213 - 96, 343 + 94, 227 - 64, 332 + 64, 554, lightMat));
    worldList.append(std::make_shared<XZRect>(0, 555, 0, 555, 0, whiteMat));
    worldList.append(std::make_shared<XZRect>(0, 555, 0, 555, 555, whiteMat));
    worldList.append(std::make_shared<XYRect>(0, 555, 0, 555, 555, whiteMat));

    std::shared_ptr<Material> box1Mat = std::make_shared<MatLambertian>(vec3(1, 1, 1));
    std::shared_ptr<Material> box2Mat = std::make_shared<MatMetal>(vec3(0.33, 0.32, 0.36), 0.01); // std::make_shared<MatLambertian>(vec3(0.33, 0.32, 0.36)); 
    std::shared_ptr<Hitable> box1 = std::make_shared<Box>(vec3(130, 0, 65), vec3(295, 165, 230), box1Mat);
    box1 = std::make_shared<RotateY>(box1, -15);
    std::shared_ptr<Hitable> box2 = std::make_shared<Box>(vec3(265, 0, 295), vec3(430, 330, 460), box2Mat);
    box2 = std::make_shared<Translate>(box2, vec3(-150, 0, 60));
    box2 = std::make_shared<RotateY>(box2, 25);
    // worldList.append(box1);
    worldList.append(box2);
    worldList.append(std::make_shared<ConstantVolume>(box1, 0.03, std::make_shared<TexSolidColour>(vec3(1, 1, 1))));
    // worldList.append(std::make_shared<ConstantVolume>(box2, 0.01, std::make_shared<TexSolidColour>(vec3(0.33, 0.32, 0.36))));

    return Camera(lookFrom, lookAt, vec3(0, 1, 0), viewFOV,
                  float(imageWidth) / float(imageHeight), aperture,
                  focusDist, timeStart, timeEnd);
}

// Get identical scene from RayTracingInOneWeekend Book 1 final image
Camera Scene::getBook1Scene() {
    // Set proper camera values
    lookFrom = vec3(13, 2, 3);
    lookAt = vec3(0, 0, 0);
    bgColour = vec3(1, 1, 1);
    useSkyColour = 2;
    viewFOV = 20;
    aperture = 0.2;
    focusDist = (lookFrom - lookAt).length();
    timeStart = 0;
    timeEnd = 1;
    
    // Generate world list
    worldList.append(std::make_shared<Sphere>(1000, vec3(0, -1000, 0), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0.5, 0.5, 0.5), std::make_shared<TexChecker>(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0))));

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
                worldList.append(std::make_shared<MovingSphere>(centre, centre2, 0.0, 1.0, randRadius, vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX))));
            }
            else if (randMat < 0.9) {
                worldList.append(std::make_shared<Sphere>(randRadius, centre, vec3(0, 0, 0), std::make_shared<MatMetal>(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX), ((float)rand() / RAND_MAX) * 0.5)));
            }
            else {
                worldList.append(std::make_shared<Sphere>(randRadius, centre, vec3(0, 0, 0), std::make_shared<MatDielectric>(((float)rand() / RAND_MAX) + 0.5)));
            }

        }
    }
    worldList.append(std::make_shared<Sphere>(1, vec3(0, 1, 0), vec3(0, 0, 0), std::make_shared<MatDielectric>(1.5)));
    worldList.append(std::make_shared<Sphere>(1, vec3(-4, 1, 0), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0.4, 0.2, 0.1))));
    worldList.append(std::make_shared<Sphere>(1, vec3(4, 1, 0), vec3(0, 0, 0), std::make_shared<MatMetal>(vec3(0.7, 0.6, 0.5), 0.1)));

    return Camera(lookFrom, lookAt, vec3(0,1,0), viewFOV, 
                  float(imageWidth) / float(imageHeight), aperture, 
                  focusDist, timeStart, timeEnd);
}

// Get identical scene from RayTracingInOneWeekend Book 2 final image
Camera Scene::getBook2Scene() {
    // Set proper camera values
    lookFrom = vec3(478, 278, -600);
    lookAt = vec3(0, 100, 0);
    bgColour = vec3(0, 0, 0);
    useSkyColour = 0;
    viewFOV = 20;
    aperture = 40.0;
    focusDist = (lookFrom - lookAt).length();
    timeStart = 0;
    timeEnd = 1;

    // Generate scene list
    int len = 0;

    // Random box floor
    auto groundMat = std::make_shared<MatLambertian>(vec3(0.48, 0.83, 0.53));
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
    worldList.append(std::make_shared<XYRect>(123, 423, 147, 412, 554, std::make_shared<DiffuseLight>(std::make_shared<TexSolidColour>(4, 4, 4))));
    worldList.append(std::make_shared<Sphere>(80, vec3(60, 280, 0), vec3(1, 0, 0), std::make_shared<DiffuseLight>(std::make_shared<TexSolidColour>(vec3(4, 4, 4)))));

    // Volumetric Fog
    auto volumeBoundaryFog = std::make_shared<Sphere>(5000, vec3(60, 280, 0), vec3(0, 0, 0), std::make_shared<MatDielectric>(1.5));
    worldList.append(std::make_shared<ConstantVolume>(volumeBoundaryFog, 0.0001, std::make_shared<TexSolidColour>(1, 1, 1)));

    // 5 Spheres
    worldList.append(std::make_shared<Sphere>(30, vec3(0, 130, 0), vec3(0, 0, 0), std::make_shared<MatDielectric>(1.5)));
    worldList.append(std::make_shared<Sphere>(30, vec3(-80, 150, 0), vec3(0, 0, 0), std::make_shared<MatDielectric>(1.5)));
    worldList.append(std::make_shared<ConstantVolume>(std::make_shared<Sphere>(30, vec3(-80, 150, 0), vec3(0, 0, 0), std::make_shared<MatDielectric>(1.5)), 0.2, std::make_shared<TexSolidColour>(0.2, 0.4, 0.7)));
    worldList.append(std::make_shared<Sphere>(30, vec3(80, 150, 0), vec3(0, 0, 0), std::make_shared<MatMetal>(vec3(0.7, 0.6, 0.5), 0.5)));
    worldList.append(std::make_shared<Sphere>(30, vec3(-160, 130, 0), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0.9, 0.8, 0.9), std::make_shared<TexPerlin>(0.04, 4, vec3(0, 0, 0), vec3(0.7, 0.6, 0.5)))));
    worldList.append(std::make_shared<ConstantVolume>(std::make_shared<Sphere>(30, vec3(160, 130, 0), vec3(0, 0, 0), std::make_shared<MatDielectric>(1.5)), 0.01, std::make_shared<TexSolidColour>(0.8, 0.3, 0.3)));

    return Camera(lookFrom, lookAt, vec3(0, 1, 0), viewFOV,
           float(imageWidth) / float(imageHeight), aperture,
           focusDist, timeStart, timeEnd);
}

// Get a scene with all available textures and materials
Camera Scene::getTextureMaterialShowcase() {
    lookFrom = vec3(12, 1.6, -6);
    lookAt = vec3(-4, -0.1, 0);
    bgColour = vec3(1, 1, 1);
    useSkyColour = 2;
    viewFOV = 20;
    aperture = 0.1;
    focusDist = (lookFrom - lookAt).length();
    timeStart = 0;
    timeEnd = 1;

    std::shared_ptr<raytrace::Texture> textureImage = std::make_shared<TexImage>("earthmap.jpg");
    std::shared_ptr<raytrace::Texture> textureWorley1 = std::make_shared<TexWorley>(8.0, vec3(0.1,0,0.2), vec3(1,1,1));
    std::shared_ptr<raytrace::Texture> textureWorley2 = std::make_shared<TexWorley>(4.0, vec3(1, 1, 1), vec3(0.2, 0.3, 0));
    std::shared_ptr<raytrace::Texture> textureChecker = std::make_shared<TexChecker>(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0);
    std::shared_ptr<raytrace::Texture> textureNoise = std::make_shared<TexNoise>(8.0, vec3(1, 1, 1));
    std::shared_ptr<raytrace::Texture> texturePerlin = std::make_shared<TexPerlin>(1.0, 1, vec3(0.94, 0.95, 0.98), vec3(0.27, 0.27, 0.43));
    std::shared_ptr<raytrace::Texture> textureMarble1 = std::make_shared<TexPerlin>(4.0, 4, vec3(0.94, 0.95, 0.98), vec3(0.27, 0.27, 0.43));
    std::shared_ptr<raytrace::Texture> textureMarble2 = std::make_shared<TexPerlin>(8.0, 8, vec3(0.94, 0.95, 0.98), vec3(0.27, 0.27, 0.43));
    std::shared_ptr<raytrace::Texture> textureStripes = std::make_shared<TexStripes>(1.0, vec3(1, 1, 1), vec3(0.2, 0.3, 0));

    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), std::make_shared<MatLambertian>(vec3(0.8, 0.3, 0.3), textureChecker)));

    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, 1), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), textureImage)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, 0), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), textureWorley2)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, -1), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), textureWorley1)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, -2), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), textureStripes)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, -3), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), textureNoise)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, -4), vec3(0, 0, 0), std::make_shared<MatDielectric>(1.33)));

    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 1, 1), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), texturePerlin)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 1, 0), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), textureMarble1)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 1, -1), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0, 0, 0), textureMarble2)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 1, -2), vec3(0, 0, 0), std::make_shared<Isotropic>(textureMarble2)));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 1, -3), vec3(0, 0, 0), std::make_shared<DiffuseLight>(std::make_shared<TexSolidColour>(vec3(4.5, 4.1, 4)))));
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 1, -4), vec3(0, 0, 0), std::make_shared<MatMetal>(vec3(1.0, 0.8, 0.9), 0.8)));

    // worldList.append(std::make_shared<YZRect>(0.15, 1.0, -0.4, 0.8, -1.5, std::make_shared<MatLambertian>(vec3(1, 0, 0), textureWorley2)));

    return Camera(lookFrom, lookAt, vec3(0, 1, 0), viewFOV,
        float(imageWidth) / float(imageHeight), aperture,
        focusDist, timeStart, timeEnd);
}

// Get super basic scene with one sphere
Camera Scene::getDebugScene() {
    lookFrom = vec3(12, 1.5, -6);
    lookAt = vec3(-2, -0.4, 0);
    bgColour = vec3(1, 1, 1);
    useSkyColour = 1;
    viewFOV = 20;
    aperture = 0.1;
    focusDist = (lookFrom - lookAt).length();
    timeStart = 0;
    timeEnd = 1;

    std::shared_ptr<raytrace::Texture> textureWorley1 = std::make_shared<TexWorley>(8.0, vec3(0.1, 0, 0.2), vec3(1, 1, 1));
    std::shared_ptr<raytrace::Texture> textureWorley2 = std::make_shared<TexWorley>(4.0, vec3(1, 1, 1), vec3(0.2, 0.3, 0));
    std::shared_ptr<raytrace::Texture> textureChecker = std::make_shared<TexChecker>(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0);
    std::shared_ptr<TexImage> texturePlain = std::make_shared<TexImage>("checkerboard.jpg"); // std::make_shared<TexSolidColour>(vec3(0.8, 0.3, 0.3));

    std::shared_ptr<NormalBase> constantNormal = std::make_shared<Constant>();
    std::shared_ptr<NormalBase> roughNormal = std::make_shared<Rough>();
    std::shared_ptr<NormalTexture> textureNormal = std::make_shared<NormalTexture>();
    textureNormal->assignTexture(texturePlain);

    std::shared_ptr<Material> matDiffuseWorley = std::make_shared<MatLambertian>(vec3(0.8, 0.3, 0.3), textureWorley1);
    std::shared_ptr<Material> matDiffuseChecker = std::make_shared<MatLambertian>(vec3(0.8, 0.3, 0.3), textureChecker);
    std::shared_ptr<Material> matMetal = std::make_shared<MatMetal>(vec3(1.0, 0.8, 0.9), 0.1);
    matMetal->assignNormalMap(constantNormal);
    std::shared_ptr<Material> matMetal2 = std::make_shared<MatMetal>(vec3(1.0, 0.8, 0.9), 0.1);
    matMetal->assignNormalMap(roughNormal);

    worldList.append(std::make_shared<Sphere>(0.55, vec3(0, 0, -1), vec3(0, 0, 0), matMetal));
    worldList.append(std::make_shared<Sphere>(0.55, vec3(2, 0, -1), vec3(0, 0, 0), matMetal2));
    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), matDiffuseChecker));
    // worldList.append(std::make_shared<YZRect>(0.15, 2.0, -0.4, 0.8, -1.5, std::make_shared<MatLambertian>(vec3(1, 0, 0), textureWorley2)));

    return Camera(lookFrom, lookAt, vec3(0, 1, 0), viewFOV,
        float(imageWidth) / float(imageHeight), aperture,
        focusDist, timeStart, timeEnd);
}


/*
// Get super basic scene with one scene
Camera Scene::getMinimalTestScene() {
    HitableList worldList;
    std::shared_ptr<raytrace::Texture> textureImage = std::make_shared<TexImage>("earthmap.jpg");
    std::shared_ptr<raytrace::Texture> textureChecker = std::make_shared<TexChecker>(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0);

    worldList.append(std::make_shared<Sphere>(0.55, vec3(0.0, 0, -1), vec3(0, 0, 0), std::make_shared<MatLambertian>(vec3(0.8, 0.3, 0.3), textureImage)));
    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), std::make_shared<MatLambertian>(vec3(0.8, 0.3, 0.3), textureChecker)));
    return worldList;
}

// Get scene with three test spheres of different materials
Camera Scene::getBaseThreeSphereScene() {
    HitableList worldList;
    worldList.append(std::make_shared<Sphere>(0.55, vec3(1.0, 0, -1), vec3(1, 0, 0), std::make_shared<MatLambertian>(vec3(0.9, 0.8, 0.9))));
    worldList.append(std::make_shared<Sphere>(0.55, vec3(-1.0, 0, -1), vec3(1, 0, 0), std::make_shared<MatMetal>(vec3(1.0, 1.0, 1.0), 0.5)));
    worldList.append(std::make_shared<Sphere>(0.55, vec3(0.0, 0, -1), vec3(1, 0, 0), std::make_shared<MatDielectric>(1.33)));
    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), std::make_shared<MatLambertian>(vec3(0.8, 0.3, 0.3), std::make_shared<TexChecker>(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0))));
    return worldList;
}

// Get scene with minimal lighting and two lights, and one sphere with a marble texture
HitableList Scene::getMinimalLightsTestScene() {
    HitableList worldList;
    worldList.append(std::make_shared<Sphere>(0.5, vec3(0, 0, -0.5), vec3(1, 0, 0), std::make_shared<MatLambertian>(vec3(0.9, 0.8, 0.9), std::make_shared<TexPerlin>(2.0, 4, vec3(0, 0, 0), vec3(0.7, 0.6, 0.5)))));
    worldList.append(std::make_shared<Sphere>(100.0, vec3(0, -100.5, -1), vec3(0, 1, 0), std::make_shared<MatLambertian>(vec3(0.8, 0.3, 0.3), std::make_shared<TexChecker>(vec3(0.8, 0.3, 0.3), vec3(1.0, 1.0, 1.0), 10.0))));
    worldList.append(std::make_shared<Sphere>(0.4, vec3(-0.4, 1.1, 0.6), vec3(1, 0, 0), std::make_shared<DiffuseLight>(std::make_shared<TexSolidColour>(vec3(4, 4, 4)))));
    worldList.append(std::make_shared<XYRect>(0.15, 1.0, -0.2, 0.5, -1.5, std::make_shared<DiffuseLight>(std::make_shared<TexSolidColour>(vec3(4, 4, 4)))));
    return worldList;
}

*/

#endif