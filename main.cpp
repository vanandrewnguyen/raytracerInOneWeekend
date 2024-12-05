#pragma once

#include "Render/render.h"

// Qt Lib
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("Toy Raytracer");
    window.resize(800, 400);
    window.show();
    return app.exec();
}





