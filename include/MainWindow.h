#ifndef GLASSYRENDERGUI_MAINWINDOW_H
#define GLASSYRENDERGUI_MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QOpenGLWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
public:
    MainWindow();
    ~MainWindow() override;
};

#endif //GLASSYRENDERGUI_MAINWINDOW_H
