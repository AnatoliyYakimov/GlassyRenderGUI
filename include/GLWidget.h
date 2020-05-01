

#ifndef GLASSYRENDERGUI_GLWIDGET_H
#define GLASSYRENDERGUI_GLWIDGET_H

#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QtOpenGL>
#include <Camera.h>

class GLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
Q_OBJECT
private:
    Camera camera;
    QOpenGLShaderProgram quadProgram;
    QOpenGLShaderProgram computeProgram;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer ebo{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLBuffer vbo{QOpenGLBuffer::Type::VertexBuffer};
    GLuint texture = 0;
    GLuint textures = 0;
    GLuint sphereSSBO = 0;
    GLuint textureSSBO = 0;
    GLuint lightSSBO = 0;

    int frameTime = 16;
    long time = clock();
    long animateTime = 0;
public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;
    QSize minimumSizeHint() const override;
    void cleanup();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void initQuadProgram();
    void initComputeProgram();
    void initTexture(int, int);
    void initSphereSSBO();
    void initTextureSSBO();
    void initLightSSBO();
};


#endif //GLASSYRENDERGUI_GLWIDGET_H
