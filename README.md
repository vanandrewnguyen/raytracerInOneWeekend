# raytracerInOneWeekend
I'm writing a custom ray-tracer based on Peter Shirley's Raytracing in One Weekend (2020 Dec 1 v3.2.2) textbook series.

I'm adding a few new features of my own to supplement my own computer graphics study.

## Feature List
- [x] Acceleration Structures (BVH)
- [x] Extended Textures (Worley, Perlin)
- [x] Volumetric Rendering
- [x] Loadable JSON Scenes
- [x] Scene Selection and File Output GUI using Qt
- [x] Sky options; fake scattering gradients
- [x] Basic Normal Mapping
- [x] Multi-threading
- [x] Importance Sampling
- [x] Denoising extension
- [ ] Checkpointing extension
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

**Normal Maps on Lambertian/Glass/Metal; Image/Rough/Smooth (~150 Samples)**
![normals could be wrong 3](https://github.com/user-attachments/assets/371142d1-268c-47bf-b1f8-df02bcdc0f96)

# Additional Features

## Loadable Scenes and QT
![gui](https://github.com/user-attachments/assets/e9cd9083-93b8-424e-b5cf-9b07d4a991e4)
- Follow the prompts to load a json scene file, or choose from a small selection of scenes (e.g. Cornell Box, Material Lighting). 
- Render time will vary significantly from image size and sample count for each pixel.
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

## Denoiser

Using techinques from "Nonlinearly Weighted First-order Regression for Denoising Monte Carlo Renderings" at 'https://benedikt-bitterli.me/nfor/', I experimented with zero and first order non local means filtering to denoise several test images. Using a similar two-pass method of collaborative filtering, I use non-local means to compute regression weights for a zero order regression (ZOR) pass, then a first order regression (FOR) pass based on strengths/weaknesses of both. The diagram below illustrates this:

![display2](https://github.com/user-attachments/assets/610a4052-7726-4bd6-94b3-96f20d5a21e6)

From left to right we have the original, ZOR, FOR, and MIX (a mixture of two passes). We sample small areas to demonstrate that ZOR successfully denoises large flat surfaces yet fails to preserve edge crispness. FOR results in noisy edges if input isn't cleaned, but preserves edges if given enough distinction. Hence, we use ZOR to clean up preliminary noise, then FOR to preserve the remaining edges given new regression weights as the variance of the first pass. We have several user parameters which are predicted through image variance, though we could perhaps study a better way to estimate this, possibly through multiple MSE checks.

An example with a larger render (LHS is raw, RHS is denoised):

![book1denoisedcmp](https://github.com/user-attachments/assets/bd566631-069c-44f0-b1e0-d21e0da86443)

## Optimisations

I've found multiple ways that have been used to parallelize ray-tracing. The method I've used is to split the canvas into rows which are picked up by the host cores. It's naive, and has some issues.
1. ~~There is a data race by trying to access the output pixel array. This causes some rows to stop rendering completely until others finish. This could potentially be fixed with parallel reduction, having each row chunk return a partial array of the full image. Once all threads finish, the final image could be rebuilt. This is a WIP.~~ Results below.
2. It's still slow. Some methods of using active cells force any idle cells to complete and for the host to continue rendering continuously. Some scenes have empty areas which cause it to render much faster. It is counter-productive to let those cells sit idle.

I ran some tests for a 500x500 debug scene, with 10 samples. Here are the results, we can see a ~30% improvement.
Threads | 1 | 2 | 4 | 8
--- | --- | --- | --- |--- 
Seconds | 24.919 | 20.750 | 19.442 | 19.102

![image](https://github.com/vanandrewnguyen/raytracerInOneWeekend/assets/53636492/26e8556a-183f-4666-99d8-e62ceaff5b49)

I fixed the data race issue described above and ran identical tests. Here are the results across multiple runs. There is a ~7% improvement.
Run | 1 | 2 | 3 | 4 | Avg
--- | --- | --- | --- | --- | ---
Before | 29.591 | 34.593 | 34.412 | 33.584 | 33.045
After | 33.640 | 30.201 | 29.505 | 30.590 | 30.984

![image](https://github.com/vanandrewnguyen/raytracerInOneWeekend/assets/53636492/8d001a63-23ac-4aff-8a88-97be2a8c2ce8)


## Importance Sampling

Following Peter Shirley's textbook series, I implemented importance sampling. You can see clear distinctions between outputs below, render computation is faster and requires lower sample count.

**Diffuse lighting (old/new) (~100 samples)**

![noise2](https://user-images.githubusercontent.com/53636492/189015814-a0b92e84-e3c3-4351-9985-f6e0b03e6e54.PNG)
![lighting](https://github.com/user-attachments/assets/c5471155-1b46-484e-8657-f880dcc347f1)

**The classic cornell box. (~50 samples)**

![cornell box](https://user-images.githubusercontent.com/53636492/189475961-15cef27e-1a86-47ac-8dcf-fe8152b86ac1.PNG)
![cornellBoxPDF](https://github.com/user-attachments/assets/74d02d50-bb97-49cf-a072-91f5c7ab14ad)

## Library Dependancies
- SDL2
- Jsoncpp
- stb_image
- Qt 6.8.0
