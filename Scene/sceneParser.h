#ifndef SCENEPARSER
#define SCENEPARSER

#include <iostream>
#include <fstream>
#include <json/json.h>

#include <sstream>
#include <memory>
#include <windows.h>

#include "../Hitables/hitable.h"
#include "../Hitables/hitableList.h"

class SceneParser {
public:
	SceneParser();

	std::string removeComments(const std::string& input);
	HitableList getWorldList(std::string& pathname, bool debugPrint = false);

	std::string jsonToString(const Json::Value& json);
	void parseData(Json::Value& root, HitableList& worldList, std::string rootName, bool debugPrint = false);
	void parseSphere(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint = false);
	void parseMovingSphere(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint = false);
	void parseBox(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint = false);
	void parseXYRect(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint = false);
	void parseYZRect(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint = false);
	void parseXZRect(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint = false);
	void parseConstantVolume(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint = false);
	std::shared_ptr<Material> createMaterial(const std::string materialType, const Json::Value& materialData, bool debugPrint = false);
	std::shared_ptr<Texture> createTexture(const std::string textureType, const Json::Value& textureData, bool debugPrint = false);
};

SceneParser::SceneParser() {}

std::string SceneParser::removeComments(const std::string& input) {
	std::istringstream stream(input);
	std::string line;
	std::string result;

	while (std::getline(stream, line)) {
		if (line.find("//") != 0) {
			result += line + '\n';
		}
	}

	return result;
}

HitableList SceneParser::getWorldList(std::string& pathname, bool debugPrint) {
	HitableList worldList;
	
	// Parse file and cleanup
	std::string path = "Scene/" + pathname;
	std::ifstream file(path);
	std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	std::string cleanFileContents = removeComments(fileContent);

	// Parse as JSON
	Json::CharReaderBuilder reader;
	Json::Value root;
	std::string errs;
	std::istringstream jsonStream(cleanFileContents);
	Json::parseFromStream(reader, jsonStream, &root, &errs);
	
	// Bottom level scene objects
	parseData(root, worldList, "Objects", debugPrint);

	return worldList;
}

void SceneParser::parseData(Json::Value& root, HitableList& worldList, std::string rootName, bool debugPrint) {
	const Json::Value& objects = root[rootName];
	for (const Json::Value& object : objects) {
		const std::string objectType = object.getMemberNames()[0];
		const Json::Value& objectData = object[objectType];

		// Case by case for each scene object
		if (objectType == "Sphere") {
			parseSphere(worldList, objectType, objectData, debugPrint);
		} else if (objectType == "MovingSphere") {
			parseMovingSphere(worldList, objectType, objectData, debugPrint);
		} else if (objectType == "Box") {
			parseBox(worldList, objectType, objectData, debugPrint);
		} else if (objectType == "ConstantVolume") {
			// parseConstantVolume(worldList, objectType, objectData, debugPrint);
		} else if (objectType == "XYRect") {
			parseXYRect(worldList, objectType, objectData, debugPrint);
		} else if (objectType == "YZRect") {
			parseYZRect(worldList, objectType, objectData, debugPrint);
		} else if (objectType == "XZRect") {
			parseXZRect(worldList, objectType, objectData, debugPrint);
		}
	}
}

void SceneParser::parseSphere(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint) {
	if (debugPrint) {
		std::cout << "Found a Sphere!" << std::endl;
	}

	const Json::Value& radius = objectData["Radius"];
	const Json::Value& origin = objectData["Origin"];
	const Json::Value& colour = objectData["Colour"];

	double originX = origin[0].asDouble();
	double originY = origin[1].asDouble();
	double originZ = origin[2].asDouble();

	double colourX = colour[0].asDouble();
	double colourY = colour[1].asDouble();
	double colourZ = colour[2].asDouble();

	// Materials... go one layer deeper
	const Json::Value& material = objectData["Material"];
	for (const std::string& materialType : material.getMemberNames()) {
		const Json::Value& materialData = material[materialType];

		std::shared_ptr<Material> mat = createMaterial(materialType, materialData, debugPrint);
		std::shared_ptr<Hitable> hit = std::make_shared<Sphere>(radius.asFloat(), vec3(originX, originY, originZ), vec3(colourX, colourY, colourZ), mat);
		
		if (debugPrint) {
			std::cout << "Added!" << std::endl;
		}

		worldList.append(hit);
	}
}

void SceneParser::parseMovingSphere(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint) {
	if (debugPrint) {
		std::cout << "Found a Moving Sphere!" << std::endl;
	}

	const Json::Value& p1 = objectData["Origin"];
	const Json::Value& p2 = objectData["Target"];
	const Json::Value& albedo = objectData["Albedo"];
	float time1 = objectData["TimeStart"].asFloat();
	float time2 = objectData["TimeEnd"].asFloat();
	float rad = objectData["Radius"].asFloat();

	float pX1 = p1[0].asFloat();
	float pY1 = p1[1].asFloat();
	float pZ1 = p1[2].asFloat();

	float pX2 = p2[0].asFloat();
	float pY2 = p2[1].asFloat();
	float pZ2 = p2[2].asFloat();

	float albedoR = albedo[0].asFloat();
	float albedoG = albedo[1].asFloat();
	float albedoB = albedo[2].asFloat();

	const Json::Value& material = objectData["Material"];
	for (const std::string& materialType : material.getMemberNames()) {
		const Json::Value& materialData = material[materialType];

		std::shared_ptr<Material> mat = createMaterial(materialType, materialData, debugPrint);
		std::shared_ptr<Hitable> hit = std::make_shared<MovingSphere>(vec3(pX1, pY1, pZ1),
																	  vec3(pX2, pY2, pZ2), 
																	  time1, 
																	  time2, 
																	  rad, 
																	  vec3(albedoR, albedoG, albedoB),
																	  mat);

		if (debugPrint) {
			std::cout << "Added!" << std::endl;
		}

		worldList.append(hit);
	}
}

void SceneParser::parseBox(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint) {
	if (debugPrint) {
		std::cout << "Found a Box!" << std::endl;
	}

	const Json::Value& p1 = objectData["Point1"];
	const Json::Value& p2 = objectData["Point2"];

	float pX1 = p1[0].asFloat();
	float pY1 = p1[1].asFloat();
	float pZ1 = p1[2].asFloat();

	float pX2 = p2[0].asFloat();
	float pY2 = p2[1].asFloat();
	float pZ2 = p2[2].asFloat();

	const Json::Value& material = objectData["Material"];
	for (const std::string& materialType : material.getMemberNames()) {
		const Json::Value& materialData = material[materialType];

		std::shared_ptr<Material> mat = createMaterial(materialType, materialData, debugPrint);
		std::shared_ptr<Hitable> hit = std::make_shared<Box>(vec3(pX1, pY1, pZ1), vec3(pX2, pY2, pZ2), mat);

		if (debugPrint) {
			std::cout << "Added!" << std::endl;
		}

		worldList.append(hit);
	}
}

void SceneParser::parseXYRect(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint) {
	if (debugPrint) {
		std::cout << "Found a XYRect!" << std::endl;
	}

	float _x0 = objectData["x0"].asFloat();
	float _x1 = objectData["x1"].asFloat();
	float _y0 = objectData["y0"].asFloat();
	float _y1 = objectData["y1"].asFloat();
	float k = objectData["Axis"].asFloat();

	const Json::Value& material = objectData["Material"];
	for (const std::string& materialType : material.getMemberNames()) {
		const Json::Value& materialData = material[materialType];

		std::shared_ptr<Material> mat = createMaterial(materialType, materialData, debugPrint);
		std::shared_ptr<Hitable> hit = std::make_shared<XYRect>(_x0, _x1, _y0, _y1, k, mat);

		if (debugPrint) {
			std::cout << "Added!" << std::endl;
		}

		worldList.append(hit);
	}
}

void SceneParser::parseYZRect(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint) {
	if (debugPrint) {
		std::cout << "Found a YZRect!" << std::endl;
	}

	float _y0 = objectData["y0"].asFloat();
	float _y1 = objectData["y1"].asFloat();
	float _z0 = objectData["z0"].asFloat();
	float _z1 = objectData["z1"].asFloat();
	float k = objectData["Axis"].asFloat();

	const Json::Value& material = objectData["Material"];
	for (const std::string& materialType : material.getMemberNames()) {
		const Json::Value& materialData = material[materialType];

		std::shared_ptr<Material> mat = createMaterial(materialType, materialData, debugPrint);
		std::shared_ptr<Hitable> hit = std::make_shared<YZRect>(_y0, _y1, _z0, _z1, k, mat);

		if (debugPrint) {
			std::cout << "Added!" << std::endl;
		}

		worldList.append(hit);
	}
}

void SceneParser::parseXZRect(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint) {
	if (debugPrint) {
		std::cout << "Found a XZRect!" << std::endl;
	}

	float _x0 = objectData["x0"].asFloat();
	float _x1 = objectData["x1"].asFloat();
	float _z0 = objectData["z0"].asFloat();
	float _z1 = objectData["z1"].asFloat();
	float k = objectData["Axis"].asFloat();

	const Json::Value& material = objectData["Material"];
	for (const std::string& materialType : material.getMemberNames()) {
		const Json::Value& materialData = material[materialType];

		std::shared_ptr<Material> mat = createMaterial(materialType, materialData, debugPrint);
		std::shared_ptr<Hitable> hit = std::make_shared<XZRect>(_x0, _x1, _z0, _z1, k, mat);

		if (debugPrint) {
			std::cout << "Added!" << std::endl;
		}

		worldList.append(hit);
	}
}

void SceneParser::parseConstantVolume(HitableList& worldList, const std::string objectType, const Json::Value& objectData, bool debugPrint) {

}

std::shared_ptr<Material> SceneParser::createMaterial(const std::string materialType, const Json::Value& materialData, bool debugPrint) {
	// Handle different material types
	if (materialType == "Lambertian") {
		if (debugPrint) {
			std::cout << "Lambertian found" << std::endl;
		}

		const Json::Value& albedo = materialData["Albedo"];
		double albedoR = albedo[0].asDouble();
		double albedoG = albedo[1].asDouble();
		double albedoB = albedo[2].asDouble();

		const Json::Value& tex = materialData["Texture"];
		for (const std::string& textureType : tex.getMemberNames()) {
			const Json::Value& textureData = tex[textureType];
			std::shared_ptr<Texture> tex = createTexture(textureType, textureData, debugPrint);
			return std::make_shared<MatLambertian>(vec3(albedoR, albedoG, albedoB), tex);
		}
	} else if (materialType == "Dielectric") {
		if (debugPrint) {
			std::cout << "Dielectric found" << std::endl;
		}

		float refraction = materialData["RefractiveIndex"].asFloat();
		return std::make_shared<MatDielectric>(refraction);
	} else if (materialType == "Metal") {
		if (debugPrint) {
			std::cout << "Metal found" << std::endl;
		}

		float reflectance = materialData["Reflectance"].asFloat();
		const Json::Value& albedo = materialData["Albedo"];
		float albedoR = albedo[0].asFloat();
		float albedoG = albedo[1].asFloat();
		float albedoB = albedo[2].asFloat();
		return std::make_shared<MatMetal>(vec3(albedoR, albedoG, albedoB), reflectance);
	} else if (materialType == "Isotropic") {
		if (debugPrint) {
			std::cout << "Isotropic found" << std::endl;
		}

		const Json::Value& tex = materialData["Texture"];
		for (const std::string& textureType : tex.getMemberNames()) {
			const Json::Value& textureData = tex[textureType];
			std::shared_ptr<Texture> tex = createTexture(textureType, textureData, debugPrint);
			return std::make_shared<Isotropic>(tex);
		}
	} else if (materialType == "DiffuseLight") {
		if (debugPrint) {
			std::cout << "Diffuse light found" << std::endl;
		}

		const Json::Value& tex = materialData["Texture"];
		for (const std::string& textureType : tex.getMemberNames()) {
			const Json::Value& textureData = tex[textureType];
			std::shared_ptr<Texture> tex = createTexture(textureType, textureData, debugPrint);
			return std::make_shared<DiffuseLight>(tex);
		}
	}

	return nullptr;
}

std::shared_ptr<Texture> SceneParser::createTexture(const std::string textureType, const Json::Value& textureData, bool debugPrint) {
	// Handle different texture types
	if (textureType == "TexImage") {
		if (debugPrint) {
			std::cout << "TexImage found" << std::endl;
		}

		const Json::Value& path = textureData["Path"];
		// Lifetime of c_str() is tmp so need to split into two variables
		std::string jsonStr = jsonToString(path);
		// Cleanup ""
		jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
		const char* cPath = jsonStr.c_str();
		return std::make_shared<TexImage>(cPath);
	} else if (textureType == "TexChecker") {
		if (debugPrint) {
			std::cout << "TexChecker found" << std::endl;
		}

		float size = textureData["Size"].asFloat();
		const Json::Value& albedo1 = textureData["Albedo1"];
		float albedoR1 = albedo1[0].asFloat();
		float albedoG1 = albedo1[1].asFloat();
		float albedoB1 = albedo1[2].asFloat();
		const Json::Value& albedo2 = textureData["Albedo2"];
		float albedoR2 = albedo2[0].asFloat();
		float albedoG2 = albedo2[1].asFloat();
		float albedoB2 = albedo2[2].asFloat();
		return std::make_shared<TexChecker>(vec3(albedoR1, albedoG1, albedoB1), vec3(albedoR2, albedoG2, albedoB2), size);
	} else if (textureType == "TexPerlin") {
		if (debugPrint) {
			std::cout << "TexPerlin found" << std::endl;
		}

		float freq = textureData["Frequency"].asFloat();
		int deg = textureData["Interval"].asInt();
		const Json::Value& albedo1 = textureData["Albedo1"];
		float albedoR1 = albedo1[0].asFloat();
		float albedoG1 = albedo1[1].asFloat();
		float albedoB1 = albedo1[2].asFloat();
		const Json::Value& albedo2 = textureData["Albedo2"];
		float albedoR2 = albedo2[0].asFloat();
		float albedoG2 = albedo2[1].asFloat();
		float albedoB2 = albedo2[2].asFloat();
		return std::make_shared<TexPerlin>(freq, deg, vec3(albedoR1, albedoG1, albedoB1), vec3(albedoR2, albedoG2, albedoB2));
	} else if (textureType == "TexSolidColour") {
		if (debugPrint) {
			std::cout << "TexSolidColour found" << std::endl;
		}

		const Json::Value& albedo = textureData["Albedo"];
		float albedoR = albedo[0].asFloat();
		float albedoG = albedo[1].asFloat();
		float albedoB = albedo[2].asFloat();
		return std::make_shared<TexSolidColour>(vec3(albedoR, albedoG, albedoB));
	}

	return nullptr;
}

std::string SceneParser::jsonToString(const Json::Value& json) {
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	return Json::writeString(builder, json);
}

#endif