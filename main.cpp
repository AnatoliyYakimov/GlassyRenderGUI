#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include <MainWindow.h>

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);

    QSurfaceFormat fmt;
    fmt.setMajorVersion(4);
    fmt.setMinorVersion(3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);

    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow mainWindow;
    mainWindow.resize(mainWindow.sizeHint());
    mainWindow.show();
    return app.exec();
}
