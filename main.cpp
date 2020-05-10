#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include <MainWindow.h>
#include <PythonQt.h>
#include <PythonQt_QtAll.h>
#include <PythonQtConversion.h>
#include <GlassyEngine.h>

#ifdef BUILD_STATIC
#include <QtPlugin>
Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);
#endif

int main(int argc, char *argv[]) {
    
    QSurfaceFormat fmt;
    fmt.setMajorVersion(4);
    fmt.setMinorVersion(3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(fmt);
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("platforms");
    QCoreApplication::setLibraryPaths(paths);
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    

    
    MainWindow *mainWindow = new MainWindow();
    
//    auto scene = new GlassyEngine(nullptr, "./shaders/compute.glsl");
//
//    PythonQt::init(PythonQt::RedirectStdOut);
//    PythonQt_QtAll::init();
//    PythonQtObjectPtr context = PythonQt::self()->getMainModule();
//    PythonQt::self()->registerClass(&Camera::staticMetaObject);
//    PythonQt::self()->registerClass(&TextureManager::staticMetaObject);
//    PythonQt::self()->registerClass(&UniformTexture::staticMetaObject);
//    PythonQt::self()->registerClass(&MappedTexture::staticMetaObject);
//    PythonQt::self()->registerClass(&TextureMap::staticMetaObject);
//    PythonQt::self()->registerClass(&Material::staticMetaObject);
//    PythonQt::self()->registerCPPClass("QMap<String, String>", "", "QtCore");
////    context.addObject("scene", scene);
//    PythonQt::self()->connect(PythonQt::self(),
//                     &PythonQt::pythonStdOut,
//                     [=](const QString &val) {
//                         qDebug(val.toUtf8());
//                     });
//    PythonQt::self()->connect(PythonQt::self(),
//                              &PythonQt::pythonStdErr,
//                              [=](const QString &val) {
//                                  qDebug(val.toUtf8());
//                              });
//    context.evalFile("./resources/scene.py");
//    qDebug(context.call("test").toString().toUtf8());


    mainWindow->show();
    return app.exec();
}
