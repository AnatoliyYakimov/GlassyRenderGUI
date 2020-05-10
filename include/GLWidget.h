

#ifndef GLASSYRENDERGUI_GLWIDGET_H
#define GLASSYRENDERGUI_GLWIDGET_H

#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QtOpenGL>
#include <Camera.h>
#include <GlassyEngine.h>

class GLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
Q_OBJECT
private:
    GlassyEngine *engine;
    QOpenGLShaderProgram quadProgram;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer ebo{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLBuffer verticiesVBO{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer textureVBO{QOpenGLBuffer::Type::VertexBuffer};
public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;
    QSize minimumSizeHint() const override;
    
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void initQuadProgram();
};


#endif //GLASSYRENDERGUI_GLWIDGET_H
