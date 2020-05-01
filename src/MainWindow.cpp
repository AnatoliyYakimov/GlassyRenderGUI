

#include <MainWindow.h>
#include <QtWidgets/QVBoxLayout>
#include <GLWidget.h>

MainWindow::MainWindow() {
    setCentralWidget(new GLWidget(this));
    setAttribute(Qt::WA_TranslucentBackground, false);
}

MainWindow::~MainWindow() {

}


