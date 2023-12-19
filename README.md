# raytracerInOneWeekend
I'm writing a custom ray-tracer using Peter Shirley's Raytracing in One Weekend textbook series.

I'm adding a few new features of my own, in my own computer graphics study.

## Feature List
- [x] Acceleration Structures (BVH)
- [x] Extended Textures (Worley, Perlin)
- [ ] Normal Mapping
- [x] Volumetric Rendering
- [x] JSON Customisable Scenes
- [x] Scene Selection and File Output CLI
- [ ] Cube Maps
- [ ] Ray-triangle intersections
- [ ] Model loading
- [ ] Atmospheric Scattering

## Sample Outputs

**Material Showcase (~100 samples)**

![materialShowcase](https://github.com/vanandrewnguyen/raytracerInOneWeekend/assets/53636492/19a6ffcd-127a-49f2-8f16-d189ed8ffea3)

**Final scene with subsurface scattering, constant volumetric mediums, caustics, boxes. (~500 samples)**

![finallargescene](https://user-images.githubusercontent.com/53636492/190929904-59f555dd-45a5-4216-9abc-a7dc5f75bfe8.PNG)

**Randomly generated spheres with defocus blur. (~100 samples)**

![checker2 (2)](https://user-images.githubusercontent.com/53636492/189015882-25888ada-3437-4f5e-b545-652d299769a9.PNG)

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

