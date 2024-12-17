#ifndef SUBSURFACEMATERIAL_H
#define SUBSURFACEMATERIAL_H

#include "material.h"

class SubsurfaceMaterial : public Material {
public:
    std::shared_ptr<raytrace::Texture> albedo;
    float scatteringCoefficient;
    float absorptionCoefficient;
    // Max iterations for random walk
    int maxSteps;
    // Minimum contribution threshold for termination
    float minContribution;
    // 0 = use all sss weight, 1.0 = use internal texture (but at this point, just use lambertian)
    float subsurfaceWeight;

public:
    SubsurfaceMaterial(
        std::shared_ptr<raytrace::Texture> a,
        float scattering,
        float absorption,
        int maxSteps = 20,
        float minContribution = 0.001f,
        float weight = 0.0f
    ) : albedo(a),
        scatteringCoefficient(scattering),
        absorptionCoefficient(absorption),
        maxSteps(maxSteps),
        minContribution(minContribution),
        subsurfaceWeight(weight) {
        /*
        scattering: how light scatters inside the material
        absorption: how much light gets absorbed inside material
        maxSteps: limits number of scattering steps
        minContribution: photons with trivial energy terminate early
        subsurfaceWeight: if material has a texture, it will be blurred by averaging process, hence we lerp tex + sss res

        e.g.
        Skin: 1.0f, 0.03f, 40, 0.001f
        Wax: 0.8f, 0.05f, 30, 0.001f (wax absorbs less light, allowing light to pass through, more translucent)
        Marble: 1.2f, 0.1f, 50, 0.0005f
        */
    }

    virtual bool scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const override;

    virtual double scatteringPdf(const Ray& rayIn, const hitRecord& rec, const Ray& scatteredRay) const override;
};

bool SubsurfaceMaterial::scatter(const Ray& rayIn, const hitRecord& rec, scatterRecord& srec) const {
    vec3 currPos = rec.pos;
    vec3 currDir = -rayIn.getDirection();
    vec3 throughput(1.0f, 1.0f, 1.0f);
    vec3 surfaceAlbedo = albedo->getColourVal(rec.u, rec.v, rec.pos);

    // Random walk SSS - step randomly into the volume and scatter light (isotropic). 
    // https://en.wikipedia.org/wiki/Subsurface_scattering
    for (int i = 0; i < maxSteps; i++) {
        // Sample random step length
        float stepLength = -log(Utility::randomDouble()) / (scatteringCoefficient + absorptionCoefficient);
        currPos += stepLength * currDir;

        // Reduce throughput exponentuially via Beer-Lambert
        throughput *= exp(-absorptionCoefficient * stepLength);

        // Photon absorbed, no scattering, stops trivial rays
        if (throughput.length() < minContribution) {
            return false;
        }

        // Check if the ray exits the surface
        currDir = randInUnitSphere();
        if (dot(currDir, rec.normal) > 0) {
            srec.specularRay = Ray(currPos, currDir);
            vec3 exitColor = albedo->getColourVal(rec.u, rec.v, currPos);
            srec.attenuation = throughput * mix(surfaceAlbedo, exitColor, subsurfaceWeight);
            srec.isSpecular = false;
            srec.pdfPtr = std::make_shared<SpherePdf>();
            return true;
        }
    }

    // Ray absorbed without exiting
    return false;
}

double SubsurfaceMaterial::scatteringPdf(const Ray& rayIn, const hitRecord& rec, const Ray& scatteredRay) const {
    // Same as isotropic mat
    return 1 / (4 * Utility::pi);
}

#endif