#ifndef MODEL_H
#define MODEL_H

#define TINYOBJLOADER_IMPLEMENTATION
#include "../tiny_obj_loader.h"

#include "hitableList.h"
#include "triangle.h"
#include "../Utils/utility.h"
#include "../Materials/lambertian.h"

// Useful links to simple obj models: https://people.sc.fsu.edu/~jburkardt/data/obj/obj.html

class Model : public Hitable {
public:
    HitableList triangles;
    AABB modelBoundingBox;

    // Constructor
    Model(const std::string& filename, std::shared_ptr<Material> material);

    virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const override;
    virtual bool boundingBox(double _time0, double _time1, AABB& outputBox) const override;
private:
    void loadModel(const std::string& filename, std::shared_ptr<Material> material);
    bool loadFile(const std::string& filename);
    bool tryOpenImmediateFile(const std::string& filename);
    std::string tryOpenFileNested(const std::string& filename);
};

Model::Model(const std::string& filename, std::shared_ptr<Material> material) {
    // Gather nested name to file (we can't guarantee location)
    std::string nestedFileName = tryOpenFileNested(filename);
    if (nestedFileName.length() == 0) {
        std::cout << "Error loading " << filename << ", aborting." << std::endl;
        throw std::runtime_error("Error loading model file " + filename);
    }
    loadModel(nestedFileName, material);

    // Compute the bounding box for the entire model
    AABB tempBox;
    if (!triangles.objectsList.empty()) {
        for (const auto& triangle : triangles.objectsList) {
            AABB triBox;
            if (triangle->boundingBox(0, 1, triBox)) {
                modelBoundingBox = AABB::surroundingBox(modelBoundingBox, triBox);
            }
        }
    }
}

bool Model::tryOpenImmediateFile(const std::string& filename) {
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filename)) {
        if (!reader.Error().empty()) {
            return false;
        }
    }

    return true;
}

std::string Model::tryOpenFileNested(const std::string& filename) {
    // Hunt for the image file in some likely locations (stolen from TexImage).
    if (tryOpenImmediateFile(filename)) return filename;
    if (tryOpenImmediateFile("Assets/" + filename)) return std::string("Assets/" + filename);
    if (tryOpenImmediateFile("../Assets/" + filename)) return std::string("../Assets/" + filename);
    if (tryOpenImmediateFile("../../Assets/" + filename)) return std::string("../../Assets/" + filename);
    if (tryOpenImmediateFile("../../../Assets/" + filename)) return std::string("../../../Assets/" + filename);
    if (tryOpenImmediateFile("../../../../Assets/" + filename)) return std::string("../../../../Assets/" + filename);

    return "";
}

void Model::loadModel(const std::string& filename, std::shared_ptr<Material> material) {
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filename)) {
        if (!reader.Error().empty()) {
            throw std::runtime_error("TinyObjReader: " + reader.Error());
        }
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    // For all shapes, break down into vert
    for (const auto& shape : shapes) {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            if (fv != 3) {
                throw std::runtime_error("Non-triangular face detected in model!");
            }

            vec3 vertices[3];
            for (int v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
                vertices[v] = vec3(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
            }
            indexOffset += fv;

            triangles.objectsList.push_back(std::make_shared<Triangle>(vertices[0], vertices[1], vertices[2], material));
        }
    }

    std::cout << "Successfully loaded model: " << filename << std::endl;
}

bool Model::hit(const Ray& r, float tMin, float tMax, hitRecord& rec) const {
    hitRecord tempRec;
    bool hitAnything = false;
    float closestSoFar = tMax;

    // Loop through all internal triangles and check for hit
    for (const auto& triangle : triangles.objectsList) {
        if (triangle->hit(r, tMin, closestSoFar, tempRec)) {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }

    return hitAnything;
}

bool Model::boundingBox(double _time0, double _time1, AABB& outputBox) const {
    outputBox = modelBoundingBox;
    return true;
}

#endif