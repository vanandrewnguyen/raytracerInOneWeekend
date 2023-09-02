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
	HitableList getWorldList(std::string& pathname);

	std::string jsonToString(const Json::Value& json);
	void parseData(Json::Value& root, HitableList& worldList, std::string rootName);
	void parseSphere(HitableList& worldList, const std::string objectType, const Json::Value& objectData);
	std::shared_ptr<Material> createMaterial(const std::string materialType, const Json::Value& materialData);
	std::shared_ptr<Texture> createTexture(const std::string textureType, const Json::Value& textureData);
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

HitableList SceneParser::getWorldList(std::string& pathname) {
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
	parseData(root, worldList, "Objects");

	return worldList;
}

void SceneParser::parseData(Json::Value& root, HitableList& worldList, std::string rootName) {
	const Json::Value& objects = root[rootName];
	for (const Json::Value& object : objects) {
		const std::string objectType = object.getMemberNames()[0];
		const Json::Value& objectData = object[objectType];

		// Case by case for each scene object
		if (objectType == "Sphere") {
			parseSphere(worldList, objectType, objectData);
		}
	}
}

void SceneParser::parseSphere(HitableList& worldList, const std::string objectType, const Json::Value& objectData) {
	const Json::Value& radius = objectData["Radius"];
	const Json::Value& origin = objectData["Origin"];
	const Json::Value& colour = objectData["Colour"];

	std::cout << "Radius: " << radius << std::endl;

	if (origin.isArray() && origin.size() == 3) {
		double originX = origin[0].asDouble();
		double originY = origin[1].asDouble();
		double originZ = origin[2].asDouble();
		std::cout << "Origin: (" << originX << ", " << originY << ", " << originZ << ")" << std::endl;
	}

	if (colour.isArray() && colour.size() == 3) {
		double colourX = colour[0].asDouble();
		double colourY = colour[1].asDouble();
		double colourZ = colour[2].asDouble();
		std::cout << "Colour: (" << colourX << ", " << colourY << ", " << colourZ << ")" << std::endl;
	}

	// Materials... go one layer deeper
	const Json::Value& material = objectData["Material"];
	for (const std::string& materialType : material.getMemberNames()) {
		const Json::Value& materialData = material[materialType];

		std::shared_ptr<Material> mat = createMaterial(materialType, materialData);
	}
}

std::shared_ptr<Material> SceneParser::createMaterial(const std::string materialType, const Json::Value& materialData) {
	// Handle different material types
	if (materialType == "MatLambertian") {
		std::cout << "Lambertian found" << std::endl;

		const Json::Value& radius = materialData["Albedo"];
		const Json::Value& tex = materialData["Texture"];
		for (const std::string& textureType : tex.getMemberNames()) {
			const Json::Value& textureData = tex[textureType];
			std::shared_ptr<Texture> tex = createTexture(textureType, textureData);
		}
	} else if (materialType == "MatDielectric") {
		std::cout << "Dielectric found" << std::endl;
	}

	return nullptr;
}

std::shared_ptr<Texture> SceneParser::createTexture(const std::string textureType, const Json::Value& textureData) {
	// Handle different texture types
	if (textureType == "TexImage") {
		std::cout << "TexImage found" << std::endl;

		const Json::Value& path = textureData["Path"];
		std::cout << path << std::endl;
		const char* cPath = jsonToString(path).c_str();
		std::cout << "Path: " << cPath << std::endl;
		return std::make_shared<TexImage>(cPath);
	} else if (textureType == "TexChecker") {

	} else if (textureType == "TexPerlin") {

	} else if (textureType == "TexSolidColour") {

	}

	return nullptr;
}

std::string SceneParser::jsonToString(const Json::Value& json)
{
	std::string result;
	Json::StreamWriterBuilder wbuilder;

	wbuilder["indentation"] = "";
	result = Json::writeString(wbuilder, json);
	return result;
}

#endif