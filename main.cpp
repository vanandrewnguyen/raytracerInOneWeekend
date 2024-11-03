#pragma once

#include "Render/render.h"

// Qt Lib
#include <QtWidgets/QApplication>
#include "Qt/gui.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("Toy Raytracer");
    window.resize(600, 200);
    window.show();
    return app.exec();
}


//namespace userInput {
//    void printPremadeScenes() {
//        std::cout << "List of premade scenes:" << std::endl;
//        for (const auto& pair : Scene::sceneMapping) {
//            std::cout << pair.first << ". " << pair.second << std::endl;
//        }
//    }
//
//    void printSceneFiles(std::string path) {
//        try {
//            for (const auto& entry : std::filesystem::directory_iterator(path)) {
//                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
//                    std::cout << "Found .txt file: " << entry.path().filename() << std::endl;
//                }
//            }
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error: " << e.what() << std::endl;
//        }
//    }
//
//    std::pair<bool, std::string> getUserCmd(int& imageWidth, int& imageHeight, int& sampleCount) {
//        int choice;
//        std::string userInput;
//        std::string w, h, ns;
//
//        while (true) {
//            std::cout << "Please choose an option:" << std::endl;
//            std::cout << "1. Render a premade scene" << std::endl;
//            std::cout << "2. Import a scene file" << std::endl;
//            std::cout << "Enter your choice: ";
//            std::cin >> choice;
//
//            if (choice == 1) {
//                std::cout << "Type 'help' for a list of scenes or enter the scene number: ";
//                std::cin >> userInput;
//
//                if (userInput == "help") {
//                    printPremadeScenes();
//                }
//                else {
//                    if (Utility::isConvertibleToInt(userInput)) {
//                        std::cout << "Image Width: ";
//                        std::cin >> w;
//                        std::cout << "Image Height: ";
//                        std::cin >> h;
//                        std::cout << "Sample Count: ";
//                        std::cin >> ns;
//                        imageWidth = atoi(w.c_str());
//                        imageHeight = atoi(h.c_str());
//                        sampleCount = atoi(ns.c_str());
//
//                        std::cout << "Selected scene number: " << userInput << std::endl;
//                        return std::pair<bool, std::string>(false, std::string(userInput));
//                    }
//                    else {
//                        break;
//                    }
//                }
//            }
//            else if (choice == 2) {
//                std::cout << "Type 'help' for a list of present files or enter the file name: ";
//                std::cin >> userInput;
//
//                if (userInput == "help") {
//                    printSceneFiles(std::string("Scene"));
//                }
//                else {
//                    return std::pair<bool, std::string>(true, std::string(userInput));
//                }
//            }
//            else {
//                std::cout << "Invalid choice. Please try again.\n";
//            }
//
//            std::cout << std::endl;
//        }
//
//        // Default
//        return std::pair<bool, std::string>(true, std::string("sceneSingleSphere.txt"));
//    }
//
//    void GetUserWriteToFile(std::vector<std::tuple<int, int, int>>& pixelData, int imageWidth, int imageHeight) {
//        std::cout << "Would you like to write the image to a file? (Y/N) ";
//        std::string userInput;
//        std::cin >> userInput;
//
//        if ((userInput == "y") || (userInput == "Y")) {
//            std::cout << "Enter file name: ";
//            std::string fileName;
//            std::cin >> fileName;
//
//            std::string tmp = std::string("./Outputs/" + fileName + ".BMP");
//            const char* name = tmp.c_str();
//
//            render::saveToBMP(name, imageWidth, imageHeight, pixelData);
//        }
//        std::cout << std::endl;
//    }
//}
//
//int main(int argc, char* argv[]) {
//    // Old Commandline style interface
//    std::mutex mutexLock;
//    int imageWidth = 200;
//    int imageHeight = 100;
//    int sampleCount = 1;
//
//    while (true) {
//        std::pair<bool, std::string> userCmd = userInput::getUserCmd(imageWidth, imageHeight, sampleCount);
//        std::vector<std::tuple<int, int, int>> pixelData;
//        if (userCmd.first) {
//            std::cout << "userCmd.second is" << userCmd.second << std::endl;
//            SceneParser parser = SceneParser();
//            Camera cam = parser.generateScene(userCmd.second, true);
//            HitableList worldList = parser.worldList;
//            render::renderScene(parser, cam, worldList, pixelData, mutexLock);
//        } else {
//            std::cout << "userCmd.second is" << userCmd.second << std::endl;
//            Scene scene = Scene();
//            Camera cam = scene.generateSceneFromMapping(atoi(userCmd.second.c_str()), imageWidth, imageHeight, sampleCount);
//            HitableList worldList = scene.worldList;
//            render::renderScene(scene, cam, worldList, pixelData, mutexLock);
//        }
//        userInput::GetUserWriteToFile(pixelData, imageWidth, imageHeight);
//    }
//
//    return 0;
//}
//



