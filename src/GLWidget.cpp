
#include <Util.h>
#include <GLWidget.h>
#include <QtGui/QImage>
#include <iostream>
#include <QtGui/qimage.h>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), camera() {
    camera.lookAt({-10, 4, 20}, {0, 2, 10}, {0, 1, 0});
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    auto logger = new QOpenGLDebugLogger(this);

    logger->initialize();

    initComputeProgram();
    initSphereSSBO();
    initTextureSSBO();
    initLightSSBO();
    initQuadProgram();
    initTexture(width(), height());

    const QList<QOpenGLDebugMessage> messages = logger->loggedMessages();
    for (const QOpenGLDebugMessage &message : messages) {
        qDebug() << message;
    }
}

void GLWidget::resizeGL(int w, int h) {
    initTexture(w, h);
    camera.recompute_size(w, h);
    QOpenGLWidget::resizeGL(w, h);
}

void GLWidget::paintGL() {
    long start = clock();
    long delta = time - start;
    animateTime += delta;
    time = start;
    computeProgram.bind();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sphereSSBO);

    QVector3D nPos = QVector3D{10 * cos(animateTime / 1000.0 * M_2_PI), 0, 10 * sin(animateTime / 1000.0 * M_2_PI) + 10};
    camera.lookAt(nPos, {0, 2, 10}, {0, 1, 0});

    QVector3D ray00 = camera.getEyeRay(camera.getLeft(), camera.getBottom());
    QVector3D ray01 = camera.getEyeRay(camera.getLeft(), camera.getTop());
    QVector3D ray11 = camera.getEyeRay(camera.getRight(), camera.getTop());
    QVector3D ray10 = camera.getEyeRay(camera.getRight(), camera.getBottom());

    computeProgram.setUniformValue("eye", camera.camera);
    computeProgram.setUniformValue("ray00", ray00.normalized());
    computeProgram.setUniformValue("ray01", ray01.normalized());
    computeProgram.setUniformValue("ray11", ray11.normalized());
    computeProgram.setUniformValue("ray10", ray10.normalized());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textures);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(width() / 16, height() / 8, 1);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sphereSSBO);
    computeProgram.release();

    ebo.bind();
    quadProgram.bind();
    vao.bind();
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    vao.release();
    ebo.release();
    quadProgram.release();
    long end = clock();
    long elapsed = end - start;
    if (elapsed < 0) {
        elapsed = 0;
    }
    QTimer::singleShot(frameTime - elapsed, [this] { this->repaint(); });
}

void GLWidget::initQuadProgram() {

    GLfloat triangles[] = {
            -1.f, -1.f, 0,
            -1.f, 1.f, 0,
            1.f, 1.f, 0,
            1.f, -1.f, 0
    };
    GLuint positions[] = {0, 2, 3, 0, 1, 2};

    quadProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders\\vertex.glsl");
    quadProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders\\fragment.glsl");
    quadProgram.link();
    vao.create();
    vbo.create();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ebo.create();
    ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vao.bind();
    vbo.bind();
    ebo.bind();
    ebo.allocate(positions, sizeof(positions));
    vbo.allocate(triangles, sizeof(triangles));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    vbo.release();
    vao.release();
    ebo.release();
}

void GLWidget::initComputeProgram() {
    QOpenGLShader *computeShader = new QOpenGLShader(QOpenGLShader::Compute);
    computeShader->compileSourceFile("shaders\\compute.glsl");
    computeProgram.create();
    computeProgram.addShader(computeShader);
    computeProgram.link();
    delete computeShader;
}

void GLWidget::initTexture(int w, int h) {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLWidget::initSphereSSBO() {
    struct Material {
        GLuint alb;
        GLuint rgh;
        GLuint ao;
        GLuint nrm;
    };
    struct Sphere {
        QVector3D center;
        GLfloat radius;
        Material mat;
    };
    Sphere spheres[] = {
            {{-4.2, 0,     11}, 1,    {0, 3, 5, 4}},
            {{-2.1, 0,     12}, 1,    {1, 3, 5, 4}},
            {{0,    0,     13}, 1,    {2, 3, 5, 4}},
            {{2.1,  0,     14}, 1,    {2, 3, 5, 4}},
            {{4.2,  0,     15}, 1,    {2, 3, 5, 4}},
            {{0,    -1004, 13}, 1000, {2, 3, 5, 4}}
    };
    glGenBuffers(1, &sphereSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sphereSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(spheres), spheres, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sphereSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GLWidget::initTextureSSBO() {
    QImage *img1 = new QImage(2048, 2048, QImage::Format::Format_RGBA8888);
    QImage *img2 = new QImage(2048, 2048, QImage::Format::Format_RGBA8888);
    QImage *img3 = new QImage(2048, 2048, QImage::Format::Format_RGBA8888);
    QImage *img4 = new QImage(2048, 2048, QImage::Format::Format_RGBA8888);
    QImage *img5 = new QImage(2048, 2048, QImage::Format::Format_RGBA8888);
    QImage *img6 = new QImage(2048, 2048, QImage::Format::Format_RGBA8888);

    assert(img1->load("resources/scuffed-plastic/scuffed-plastic2-alb.png", "PNG"));
    assert(img2->load("resources/scuffed-plastic/scuffed-plastic3-alb.png", "PNG"));
    assert(img3->load("resources/scuffed-plastic/scuffed-plastic4-alb.png", "PNG"));
    assert(img4->load("resources/scuffed-plastic/scuffed-plastic-rough.png", "PNG"));
    assert(img5->load("resources/scuffed-plastic/scuffed-plastic-normal.png", "PNG"));
    assert(img6->load("resources/scuffed-plastic/scuffed-plastic8-alb.png", "PNG"));
    img6->reinterpretAsFormat(QImage::Format::Format_RGBA8888);


    int w = 2048;
    int h = 2048;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &textures);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textures);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, w, h, 6);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, w, h, 6, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, nullptr);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, w, h, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, img1->bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, w, h, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, img2->bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, w, h, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, img3->bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, w, h, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, img4->bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 4, w, h, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, img5->bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 5, w, h, 1, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, img6->bits());
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    delete img1;
    delete img2;
    delete img3;
    delete img4;
    delete img5;
    delete img6;

}


void GLWidget::initLightSSBO() {
    struct Light {
        QVector3D col;
        GLfloat intesity;
        QVector3D v;
        GLint type;
    };

    int sz = sizeof(Light);

    Light lights[] = {
            {{1, 1, 1}, 0.8, {-10, 2,  0}, 0},
            {{1, 1, 1}, 3,   {-1,  -1, 1}, 1}
    };


    glGenBuffers(1, &lightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(lights), lights, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

GLWidget::~GLWidget() {

}

void GLWidget::cleanup() {
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &textureSSBO);
    glDeleteBuffers(1, &sphereSSBO);
    glDeleteBuffers(1, &lightSSBO);
}

QSize GLWidget::minimumSizeHint() const {
    return QSize(800, 800);
}



