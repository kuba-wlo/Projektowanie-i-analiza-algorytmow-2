#include "MainWindow.h"

#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(600, 700);
    window.show();

    return app.exec();
}
