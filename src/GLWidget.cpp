#include <Util.h>
#include <GLWidget.h>
#include <QtGui/QImage>
#include <iostream>
#include <QtGui/qimage.h>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    
    auto logger = new QOpenGLDebugLogger(this);
    
    logger->initialize();
    
    initQuadProgram();
    
    engine = new GlassyEngine(this, "./shaders/compute.glsl");
    engine->getEngineContext()->setShareContext(context());
    context()->setShareContext(engine->getEngineContext());
    engine->connect(engine, &GlassyEngine::frameRenderingDone, [this](){this->update();});
    engine->renderFrame();
}

void GLWidget::resizeGL(int w, int h) {
    engine->renderFrame();
    QOpenGLWidget::resizeGL(w, h);
}

void GLWidget::paintGL() {
    quadProgram.bind();
    vao.bind();
    engine->bindCanvas();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    engine->unbindCanvas();
    vao.release();
    quadProgram.release();
    
}

void GLWidget::initQuadProgram() {
    
    GLfloat triangles[] = {
        -1.f, -1.f, 0,
        -1.f,  1.f, 0,
         1.f,  1.f, 0,
         1.f, -1.f, 0
    };
    
    GLfloat textures[] = {
        0, 0,
        0, 0.5,
        1, 0.5,
        1, 0
    };
    GLuint positions[] = {0, 2, 3, 0, 1, 2};
    
    quadProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders\\vertex.glsl");
    quadProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders\\fragment.glsl");
    quadProgram.link();
    vao.create();
    verticiesVBO.create();
    verticiesVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ebo.create();
    ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vao.bind();
    verticiesVBO.bind();
    ebo.bind();
    ebo.allocate(positions, sizeof(positions));
    verticiesVBO.allocate(triangles, sizeof(triangles));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
    verticiesVBO.release();
    textureVBO.create();
    textureVBO.bind();
    textureVBO.allocate(textures, sizeof(textures));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    textureVBO.release();
    vao.release();
    ebo.release();
}

GLWidget::~GLWidget() {

}

QSize GLWidget::minimumSizeHint() const {
    return QSize(800, 600);
}



