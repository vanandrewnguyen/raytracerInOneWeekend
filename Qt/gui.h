//#ifndef QTGUI_H
//#define QTGUI_H
//
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QMainWindow>
//#include <QtWidgets/QTabWidget>
//#include <QtWidgets/QWidget>
//#include <QtWidgets/QVBoxLayout>
//#include <QtWidgets/QPushButton>
//#include <QtWidgets/QLineEdit>
//#include <QtWidgets/QFileDialog>
//#include <QtWidgets/QLabel>
//#include <QtWidgets/QListWidget>
//#include <QtWidgets/QButtonGroup>
//#include <QtWidgets/QRadioButton>
//#include <QtGui/QIntValidator>
//
//#include <iostream>
//
//// #include "../Render/render.h"
//
//class MainWindow : public QMainWindow {
//public:
//    std::vector<std::tuple<int, int, int>> pixelData;
//    int imageWidth;
//    int imageHeight;
//    int sampleCount;
//    std::string sceneName;
//
//    QLineEdit* widthInput;
//    QLineEdit* heightInput;
//    QLineEdit* sampleCountInput;
//    QButtonGroup* sceneGroup;
//    QLabel* filePathLabel;
//
//    // Todo: move this into a utility file or data file
//    static inline std::map<int, std::string> sceneMapping = {
//        {1, "Cornell Box"},
//        {2, "Infinite Spheres on Checkerboard"},
//        {3, "Spheres and Cubes in Shadow"},
//        {4, "Material/Texture Showcase"},
//        {5, "Debug Scene"}
//    };
//
//public:
//    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
//        QTabWidget* tabWidget = new QTabWidget(this);
//
//        // Tab 1: User Input
//        QWidget* inputTab = new QWidget();
//        QVBoxLayout* inputLayout = new QVBoxLayout(inputTab);
//
//        int defaultImageWidth = 400;
//        int defaultImageHeight = 400;
//        int defaultSampleCount = 5;
//        widthInput = new QLineEdit(QString::number(defaultImageWidth));
//        heightInput = new QLineEdit(QString::number(defaultImageHeight));
//        sampleCountInput = new QLineEdit(QString::number(defaultSampleCount));
//        imageWidth = defaultImageWidth;
//        imageHeight = defaultImageHeight;
//        sampleCount = defaultSampleCount;
//
//        // Ensure values are purely numerical
//        QIntValidator* validator = new QIntValidator(1, 10000, this);
//        widthInput->setValidator(validator);
//        heightInput->setValidator(validator);
//        sampleCountInput->setValidator(validator);
//
//        connect(widthInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
//        connect(heightInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
//        connect(sampleCountInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
//
//        inputLayout->addWidget(new QLabel("Image Width:"));
//        inputLayout->addWidget(widthInput);
//        inputLayout->addWidget(new QLabel("Image Height:"));
//        inputLayout->addWidget(heightInput);
//        inputLayout->addWidget(new QLabel("Sample Count:"));
//        inputLayout->addWidget(sampleCountInput);
//
//        // Tab 2: Premade Scene Selection
//        QWidget* premadeSceneTab = new QWidget();
//        QVBoxLayout* sceneLayout = new QVBoxLayout(premadeSceneTab);
//        sceneLayout->addWidget(new QLabel("Choose from Premade Scenes:"));
//        sceneGroup = new QButtonGroup(premadeSceneTab);
//        
//        for (const auto& pair : sceneMapping) {
//            QString qstr = QString::fromUtf8(pair.second.c_str());
//            QRadioButton* radioButton = new QRadioButton(qstr);
//            radioButton->setProperty("sceneId", pair.first);
//            connect(radioButton, &QRadioButton::toggled, this, &MainWindow::updateSceneName);
//
//            sceneLayout->addWidget(radioButton);
//            sceneGroup->addButton(radioButton);
//        }
//
//        QPushButton* startRenderButtonPremade = new QPushButton("Start Render");
//        sceneLayout->addWidget(startRenderButtonPremade);
//
//        // Tab 3: Load Scene from JSON
//        QWidget* userSceneTab = new QWidget();
//        QVBoxLayout* txtLayout = new QVBoxLayout(userSceneTab);
//
//        QPushButton* loadFileButton = new QPushButton("Load Scene File");
//        filePathLabel = new QLabel("No file selected.");
//
//        connect(loadFileButton, &QPushButton::clicked, [=]() {
//            QString filePath = QFileDialog::getOpenFileName(this, "Open Scene File", "", "Text Files (*.txt)");
//            if (!filePath.isEmpty()) {
//                filePathLabel->setText(filePath);
//                sceneName = filePath.toStdString();
//            }
//            });
//
//        QPushButton* startRenderButtonLoaded = new QPushButton("Start Render");
//        txtLayout->addWidget(new QLabel("Load Custom Scene:"));
//        txtLayout->addWidget(loadFileButton);
//        txtLayout->addWidget(filePathLabel);
//        txtLayout->addWidget(startRenderButtonLoaded);
//
//        // Connect tabs and buttons
//        connect(startRenderButtonPremade, &QPushButton::clicked, this, &MainWindow::onStartRenderButtonPremadeClicked);
//        connect(startRenderButtonLoaded, &QPushButton::clicked, this, &MainWindow::onStartRenderButtonLoadedClicked);
//
//        tabWidget->addTab(inputTab, "Render Settings");
//        tabWidget->addTab(premadeSceneTab, "Premade Scenes");
//        tabWidget->addTab(userSceneTab, "Load Scene");
//
//        // Set central widget
//        setCentralWidget(tabWidget);
//    }
//
//private slots:
//    void updateSceneName() {
//        QAbstractButton* checkedButton = sceneGroup->checkedButton();
//        if (checkedButton) {
//            sceneName = checkedButton->text().toUtf8();
//        }
//    }
//
//    void updateSettings() {
//        imageWidth = widthInput->text().toInt();
//        imageHeight = heightInput->text().toInt();
//        sampleCount = sampleCountInput->text().toInt();
//    }
//
//    void onStartRenderButtonPremadeClicked() {
//        std::cout << "Rendering premade scene: " << sceneName
//            << " with width: " << imageWidth
//            << ", height: " << imageHeight
//            << ", samples: " << sampleCount << std::endl;
//
//        /*SceneParser parser = SceneParser();
//        Camera cam = parser.generateScene(userCmd.second, true);
//        HitableList worldList = parser.worldList;
//        render::renderScene(parser, cam, worldList, pixelData);*/
//    }
//
//    void onStartRenderButtonLoadedClicked() {
//        /*Scene scene = Scene();
//        Camera cam = scene.generateSceneFromMapping(atoi(userCmd.second.c_str()), imageWidth, imageHeight, sampleCount);
//        HitableList worldList = scene.worldList;
//        render::renderScene(scene, cam, worldList, pixelData);*/
//    }
//};
//
//#endif
