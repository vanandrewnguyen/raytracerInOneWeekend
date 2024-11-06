#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include "../init.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QMessageBox>
#include <QtGui/QIntValidator>

#include <iostream>

// Const
// Max depth is the maximum number of bounces a ray can have before destroying itself

namespace render {
    vec3 scene(const Ray& r, vec3& bgCol, HitableList world, int depth, int useSkyCol) {
        // Make new list of world items
        hitRecord rec;
        if (depth >= Utility::maxDepth) {
            return vec3(0, 0, 0);
        }

        if (!(world.hit(r, 0.001, Utility::maxFloat, rec))) {
            // If no collision, return sky colour
            switch (useSkyCol) {
            case 0:
                return bgCol;
                break;
            case 1:
                return Utility::getFakeScatteringSkyGradient(r);
                break;
            case 2:
                return Utility::getSimpleSkyGradient(r);
                break;
            case 3:
                return Utility::getNightSkyGradient(r);
                break;
            default:
                return bgCol;
            }
        }

        // Else, do our recursive calls
        Ray scatteredRay;
        vec3 attenuation;
        vec3 emitted = rec.matPtr->emitted(rec.u, rec.v, rec.pos);
        // Push off the surface normal a little bit (no collision error)
        if (!(rec.matPtr->scatter(r, rec, attenuation, scatteredRay))) {
            return emitted;
        }

        return emitted + attenuation * scene(scatteredRay, bgCol, world, depth + 1, useSkyCol);

        /*
        if (depth < MAXDEPTH && rec.matPtr->scatter(r, rec, attenuation, scatteredRay)) {
            return attenuation * scene(scatteredRay, world, depth + 1);
        } else {
            return vec3(0, 0, 0);
        }*/
    }

    void writeColourToScreen(int imgWidth, int imgHeight, Camera& cam, int x, int y, HitableList world, int sampleCount, vec3& bgCol, int useSkyCol, std::vector<std::tuple<int, int, int>>& pixelData) {
        // Set UV's
        // We can offset randomly to anti alias cheaply, moving the cam
        vec3 col(0, 0, 0);
        for (int s = 0; s < sampleCount; s++) {
            float u = float(x + (float)rand() / RAND_MAX) / float(imgWidth);
            float v = float(y + (float)rand() / RAND_MAX) / float(imgHeight);

            // Get sky colour
            Ray rayDir = cam.getRay(u, v);
            vec3 pos = rayDir.getPointParam(2.0);
            col += scene(rayDir, bgCol, world, 0, useSkyCol);
        }
        // Divide by sample count
        col /= float(sampleCount);

        // Colour gamma correction
        col = vec3(sqrt(col.getX()), sqrt(col.getY()), sqrt(col.getZ()));

        // Normalize values
        int ir = Utility::clamp(static_cast<int>(255.999 * col.getX()), 0, 255);
        int ig = Utility::clamp(static_cast<int>(255.999 * col.getY()), 0, 255);
        int ib = Utility::clamp(static_cast<int>(255.999 * col.getZ()), 0, 255);

        // Write in real time
        if (x % imgWidth == 1) sdltemplate::loop();

        // Output
        sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
        sdltemplate::drawPoint(x, imgHeight - y);

        // Write to pixel data
        pixelData.emplace_back(ir, ig, ib);
    }

    std::vector<std::tuple<int, int, int>> renderChunk(Scene& parser, Camera& cam, HitableList& worldList, int startRow, int endRow, std::mutex& mutex, bool debugPrint = false) {
        // Return a partial array of the final image
        std::vector<std::tuple<int, int, int>> partialPixelData;

        // Render portion of the image
        for (int y = endRow - 1; y >= startRow; y--) {
            for (int x = 0; x < parser.imageWidth; x++) {
                // Output (remember your thread safety)
                std::lock_guard<std::mutex> lock(mutex);
                writeColourToScreen(parser.imageWidth, parser.imageHeight, cam, x, y, worldList, parser.sampleCount, parser.bgColour, parser.useSkyColour, partialPixelData);

                if (debugPrint) {
                    std::cout << "Rendering pixel " << int(x + y) << std::endl;
                }
            }
        }

        return partialPixelData;
    }

    void renderScene(Scene& parser, Camera& cam, HitableList& worldList, std::vector<std::tuple<int, int, int>>& pixelData, std::mutex& mutex, bool useMultithread = true, bool debugPrint = false) {
        srand((unsigned)time(NULL));

        // Establish SDL Window
        sdltemplate::sdl("Raytracer", parser.imageWidth, parser.imageHeight);
        sdltemplate::loop();
        std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
        
        if (!useMultithread) {
            // Render the entire scene in a single loop (no threads)
            pixelData = renderChunk(parser, cam, worldList, 0, parser.imageHeight, mutex, debugPrint);
        } else {
            // Get available CPU cores (8)
            const int numThreads = std::thread::hardware_concurrency();

            // Create and launch threads
            // We are splitting the image into 'n' rows instead of using active 'n' cells method
            std::vector<std::thread> threads;
            int rowsPerThread = parser.imageHeight / numThreads;
            int startRow = 0;
            int endRow = rowsPerThread;

            // Vector to store partial pixel data and their start height
            std::vector<std::pair<int, std::vector<std::tuple<int, int, int>>>> partialResults;

            for (int i = 0; i < numThreads; i++) {
                threads.emplace_back([&, i, startRow, endRow]() {
                    std::vector<std::tuple<int, int, int>> partialPixelData = renderChunk(std::ref(parser),
                        std::ref(cam),
                        std::ref(worldList),
                        startRow,
                        endRow,
                        mutex,
                        debugPrint);
                    partialResults.emplace_back(startRow, std::move(partialPixelData));
                    });

                startRow = endRow;
                endRow = (i == numThreads - 2) ? parser.imageHeight : endRow + rowsPerThread;
            }

            // Join threads, wait
            for (std::thread& thread : threads) {
                thread.join();
            }

            // Combine partial results into the final pixelData without sorting, since we know order
            // Parallel reduction -> fix the data race of multiple threads trying to write to pixelData simultaneously
            // Image is reconstructed at the end
            for (const auto& partialResult : partialResults) {
                pixelData.insert(pixelData.end(), partialResult.second.begin(), partialResult.second.end());
            }
        }
    
        // Print time taken
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        Utility::printTimeTaken(startTime, endTime);

        if (debugPrint) {
            std::cout << "Done!" << std::endl;
        }

        // Keep window active
        while (sdltemplate::running) {
            // Handle events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    sdltemplate::running = false;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        sdltemplate::running = false;
                    }
                }
            }

            sdltemplate::loop();
        }

        SDL_Quit();
    }

    void saveToBMP(const char* filename, int width, int height, const std::vector<std::tuple<int, int, int>>& pixels) {
        std::vector<uint8_t> bmpData; // Create a vector to store the BMP data

        // Convert the RGB pixel data to BMP format (BGR)
        for (const auto& pixel : pixels) {
            uint8_t b = static_cast<uint8_t>(std::get<0>(pixel));
            uint8_t g = static_cast<uint8_t>(std::get<1>(pixel));
            uint8_t r = static_cast<uint8_t>(std::get<2>(pixel));

            bmpData.push_back(b);
            bmpData.push_back(g);
            bmpData.push_back(r);
        }

        // Use stb_image_write to save the BMP file
        if (!stbi_write_bmp(filename, width, height, 3, bmpData.data())) {
            std::cerr << "Failed to save the BMP image." << std::endl;
        }
    }
}

class MainWindow : public QMainWindow {
public:
    std::vector<std::tuple<int, int, int>> pixelData;
    std::mutex mutexLock;

    int imageWidth;
    int imageHeight;
    int sampleCount;
    std::string sceneName;
    bool useMultithread;

    QLineEdit* widthInput;
    QLineEdit* heightInput;
    QLineEdit* sampleCountInput;
    QButtonGroup* sceneGroup;
    QLabel* filePathLabel;
    QRadioButton* singleThreadedButton;
    QRadioButton* multiThreadedButton;

    // Todo: move this into a utility file or data file
    static inline std::map<int, std::string> sceneMapping = {
        {1, "Cornell Box"},
        {2, "Infinite Spheres on Checkerboard"},
        {3, "Spheres and Cubes in Shadow"},
        {4, "Material/Texture Showcase"},
        {5, "Debug Scene"}
    };

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        QTabWidget* tabWidget = new QTabWidget(this);

        // Tab 1: User Input
        QWidget* inputTab = new QWidget();
        QVBoxLayout* inputLayout = new QVBoxLayout(inputTab);

        QLabel* sizeLabel = new QLabel("Render Output options:");
        inputLayout->addWidget(sizeLabel);

        int defaultImageWidth = 400;
        int defaultImageHeight = 400;
        int defaultSampleCount = 5;
        widthInput = new QLineEdit(QString::number(defaultImageWidth));
        heightInput = new QLineEdit(QString::number(defaultImageHeight));
        sampleCountInput = new QLineEdit(QString::number(defaultSampleCount));
        imageWidth = defaultImageWidth;
        imageHeight = defaultImageHeight;
        sampleCount = defaultSampleCount;

        // Ensure values are purely numerical
        QIntValidator* validator = new QIntValidator(1, 10000, this);
        widthInput->setValidator(validator);
        heightInput->setValidator(validator);
        sampleCountInput->setValidator(validator);

        connect(widthInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
        connect(heightInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
        connect(sampleCountInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);

        inputLayout->addWidget(new QLabel("Image Width:"));
        inputLayout->addWidget(widthInput);
        inputLayout->addWidget(new QLabel("Image Height:"));
        inputLayout->addWidget(heightInput);
        inputLayout->addWidget(new QLabel("Sample Count:"));
        inputLayout->addWidget(sampleCountInput);

        // Threading options
        QLabel* threadingLabel = new QLabel("Processing options:");
        inputLayout->addWidget(threadingLabel);

        QButtonGroup* threadingGroup = new QButtonGroup(inputTab);
        singleThreadedButton = new QRadioButton("Single Threaded");
        multiThreadedButton = new QRadioButton("Multithreaded");
        multiThreadedButton->setChecked(true);
        useMultithread = true;
        connect(singleThreadedButton, &QRadioButton::toggled, this, &MainWindow::updateSettings);
        connect(multiThreadedButton, &QRadioButton::toggled, this, &MainWindow::updateSettings);

        threadingGroup->addButton(singleThreadedButton);
        threadingGroup->addButton(multiThreadedButton);

        inputLayout->addWidget(singleThreadedButton);
        inputLayout->addWidget(multiThreadedButton);

        // Tab 2: Premade Scene Selection
        QWidget* premadeSceneTab = new QWidget();
        QVBoxLayout* sceneLayout = new QVBoxLayout(premadeSceneTab);
        sceneLayout->addWidget(new QLabel("Choose from Premade Scenes:"));
        sceneGroup = new QButtonGroup(premadeSceneTab);

        for (const auto& pair : sceneMapping) {
            QString qstr = QString::fromUtf8(pair.second.c_str());
            QRadioButton* radioButton = new QRadioButton(qstr);
            radioButton->setProperty("sceneId", pair.first);
            connect(radioButton, &QRadioButton::toggled, this, &MainWindow::updateSceneName);

            sceneLayout->addWidget(radioButton);
            sceneGroup->addButton(radioButton);
        }

        QPushButton* startRenderButtonPremade = new QPushButton("Start Render");
        sceneLayout->addWidget(startRenderButtonPremade);

        // Tab 3: Load Scene from JSON
        QWidget* userSceneTab = new QWidget();
        QVBoxLayout* txtLayout = new QVBoxLayout(userSceneTab);

        QPushButton* loadFileButton = new QPushButton("Load Scene File");
        filePathLabel = new QLabel("No file selected.");
        connect(loadFileButton, &QPushButton::clicked, this, &MainWindow::openLoadedScene);

        QPushButton* startRenderButtonLoaded = new QPushButton("Start Render");
        txtLayout->addWidget(new QLabel("Load Custom Scene:"));
        txtLayout->addWidget(loadFileButton);
        txtLayout->addWidget(filePathLabel);
        txtLayout->addWidget(startRenderButtonLoaded);

        // Connect tabs and buttons
        connect(startRenderButtonPremade, &QPushButton::clicked, this, &MainWindow::startPremadeRender);
        connect(startRenderButtonLoaded, &QPushButton::clicked, this, &MainWindow::startLoadedRender);

        tabWidget->addTab(inputTab, "Render Settings");
        tabWidget->addTab(premadeSceneTab, "Premade Scenes");
        tabWidget->addTab(userSceneTab, "Load Scene");

        // Set central widget
        setCentralWidget(tabWidget);

        // Stylesheets
        QString darkStyle = R"(
    QMainWindow {
        background-color: #121212;
        color: #ffffff;
    }
    
    QWidget {
        background-color: #1e1e1e;
        color: #ffffff;
    }
    
    QLabel {
        color: #ffffff;
    }

    QLineEdit {
        background-color: #333333;
        color: #ffffff;
        border: 1px solid #444444;
        border-radius: 5px;
        padding: 5px;
    }
    
    QLineEdit:focus {
        border: 1px solid #0078d7; /* Focused color */
    }

    QPushButton {
        background-color: #444444;
        color: #ffffff;
        border: 1px solid #555555;
        border-radius: 5px;
        padding: 5px;
    }
    
    QPushButton:hover {
        background-color: #555555;
    }
    
    QPushButton:pressed {
        background-color: #666666;
    }
    
    QRadioButton {
        color: #ffffff;
    }
    
    QRadioButton::indicator {
        background-color: #333333;
        border: 1px solid #444444;
    }
    
    QRadioButton::indicator:checked {
        background-color: #0078d7;
        border: 1px solid #0078d7;
    }

    QButtonGroup {
        margin-top: 10px;
    }
    
    QTabWidget::pane {
        background: #1e1e1e; /* Background for the tab widget */
        border: 1px solid #444444;
    }

    QTabBar::tab {
        background: #333333;
        color: #ffffff;
        padding: 10px;
    }
    
    QTabBar::tab:selected {
        background: #0078d7;
        color: #ffffff;
    }
    
    QProgressBar {
        border: 1px solid #555555;
        border-radius: 5px;
        background: #444444;
    }

    QProgressBar::chunk {
        background-color: #0078d7;
        border-radius: 5px;
    }
)";
        this->setStyleSheet(darkStyle);
    }

private:
    void promptSaveImage() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Save Image", "Would you like to save the image to a file?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "Images (*.bmp);;All Files (*)");
            if (!fileName.isEmpty()) {
                const char* name = fileName.toUtf8().constData();
                render::saveToBMP(name, imageWidth, imageHeight, pixelData);
            }
        }
    }

private slots:
    void updateSceneName() {
        QAbstractButton* checkedButton = sceneGroup->checkedButton();
        if (checkedButton) {
            sceneName = checkedButton->text().toUtf8().constData();
        }
    }

    void updateSettings() {
        imageWidth = widthInput->text().toInt();
        imageHeight = heightInput->text().toInt();
        sampleCount = sampleCountInput->text().toInt();
        useMultithread = multiThreadedButton->isChecked();
    }

    void startPremadeRender() {
        std::cout << "Rendering premade scene: " << sceneName
            << " with width: " << imageWidth
            << ", height: " << imageHeight
            << ", samples: " << sampleCount << std::endl;

        int index = 0;
        for (const auto& pair : sceneMapping) {
            if (pair.second == sceneName) {
                index = pair.first;
                break;
            }
        }

        Scene scene = Scene();
        Camera cam = scene.generateSceneFromMapping(index, imageWidth, imageHeight, sampleCount);
        HitableList worldList = scene.worldList;
        render::renderScene(scene, cam, worldList, pixelData, mutexLock, useMultithread);

        promptSaveImage();
    }

    void startLoadedRender() {
        std::cout << "Rendering loaded scene: " << sceneName
            << " with width: " << imageWidth
            << ", height: " << imageHeight
            << ", samples: " << sampleCount << std::endl;

        SceneParser parser = SceneParser();
        Camera cam = parser.generateScene(sceneName, true);
        HitableList worldList = parser.worldList;
        render::renderScene(parser, cam, worldList, pixelData, mutexLock, useMultithread);

        promptSaveImage();
    }

    void openLoadedScene() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open Scene File", "", "Text Files (*.txt)");
        if (!filePath.isEmpty()) {
            QFileInfo fileInfo(filePath);
            QString fileName = fileInfo.fileName();
            filePathLabel->setText(fileName);
            sceneName = fileName.toUtf8().constData();
        }
    }
};

#endif