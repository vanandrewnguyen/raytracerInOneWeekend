# raytracerInOneWeekend
I'm writing a custom ray-tracer using Peter Shirley's Raytracing in One Weekend textbook series.

I'm adding a few new features of my own, in my own computer graphics study.

## Feature List
- [x] Acceleration Structures (BVH)
- [x] Extended Textures (Worley, Perlin)
- [x] Volumetric Rendering
- [x] JSON Customisable Scenes (Read from file)
- [x] Scene Selection and File Output CLI
- [x] Sky options; fake scattering gradients
- [x] (Super) Basic Normal Mapping
- [x] Multi-threading
- [ ] Ray-triangle intersections
- [ ] Model loading


## Sample Outputs
**Final scene with subsurface scattering, constant volumetric mediums, caustics, boxes. (~500 samples)**

![book2](https://github.com/vanandrewnguyen/raytracerInOneWeekend/assets/53636492/39f55032-0a6c-491b-82b5-6aaa99274ad7)

**Material Showcase (~100 samples)**
(Isotropic, Perlin Noise, Worley Noise, Dielectrics, Metallics, Image Textures)

![materialShowcase](https://github.com/vanandrewnguyen/raytracerInOneWeekend/assets/53636492/19a6ffcd-127a-49f2-8f16-d189ed8ffea3)

**Randomly generated spheres with defocus blur. (~100 samples)**

![checker2 (2)](https://user-images.githubusercontent.com/53636492/189015882-25888ada-3437-4f5e-b545-652d299769a9.PNG)

**Constant Normal Map + Simple Rough Normal Map (~30 Samples)**
![normalMapBasic](https://github.com/vanandrewnguyen/raytracerInOneWeekend/assets/53636492/086d85e1-0605-4c9e-807f-c53733cda58f)

**Basic diffuse lighting. (~100 samples)**

![noise2](https://user-images.githubusercontent.com/53636492/189015814-a0b92e84-e3c3-4351-9985-f6e0b03e6e54.PNG)

**The classic cornell box. (~50 samples)**

![cornell box](https://user-images.githubusercontent.com/53636492/189475961-15cef27e-1a86-47ac-8dcf-fe8152b86ac1.PNG)

## Directions for Use
- Follow the prompts to load a json scene file, or choose from a small selection of scenes (e.g. Cornell Box, Material Lighting). 
- Render time will vary significantly from image size and sample count for each pixel.
- After the image finishes rendering, you can close the render window.
- You will be prompted whether to save the output to an image, which will be saved in "./Outputs/".
- A small list of sample json scenes have been provided in "./Scenes/", as well as a reference manual for all supported objects.

An example of an editable json scene.
```
{
  "Camera": {
    "LookFrom": [-3.0, 3.0, -2.0],
		"LookAt": [0.0, -0.1, -1.0],
		"UpVector": [0.0, 1.0, 0.0],
		"ViewFOV": 20.0,
		"Aperture": 0.1,
		"FocusDist": -100.0,
		"TimeStart": 0.0,
		"TimeEnd": 1.0,
	},
	"Settings": {
		"ImageWidth": 800,
		"ImageHeight": 400,
		"SampleCount": 1,
		"BackgroundColour": [1.0, 1.0, 1.0],
		"UseOutdoorLighting": 1,
  }, 
  "Objects": [
    {
    "Type": "Sphere",
    "Data": {
        "Radius": 0.55,
        "Origin": [0.0, 0.0, -1.0],
        "Colour": [0.0, 0.0, 0.0],
        "Material": {
          "Lambertian": {
            "Albedo": [0.8, 0.3, 0.3],
            "Texture": {
              "TexImage": {
                "Path": "earthmap.jpg",
              }
            }
          },
        },
      },
    },
    ]
}
```


## Library Dependancies
- SDL2
- Jsoncpp
- stb_image

## Additional Notes
I've found multiple ways that have been used to parallelize ray-tracing. The method I've used is to split the canvas into rows which are picked up by the host cores. It's naive, and has some issues.
1. There is a data race by trying to access the output pixel array. This causes some rows to stop rendering completely until others finish. This could potentially be fixed with parallel reduction, having each row chunk return a partial array of the full image. Once all threads finish, the final image could be rebuilt. This is a WIP.
2. It's still slow. Some methods of using active cells force any idle cells to complete and for the host to continue rendering continuously. Some scenes have empty areas which cause it to render much faster. It is counter-productive to let those cells sit idle.

I ran some tests for a 500x500 debug scene, with 10 samples. Here are the results, we can see a ~1.3x improvement.
Threads | #1 | #2 | #4 | #8
--- | --- | --- | --- |--- 
Seconds | 24.919 | 20.750 | 19.442 | 19.102

![image](https://github.com/vanandrewnguyen/raytracerInOneWeekend/assets/53636492/26e8556a-183f-4666-99d8-e62ceaff5b49)
