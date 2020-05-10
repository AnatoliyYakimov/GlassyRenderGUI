#include <GlassyEngine.h>

GlassyEngine::GlassyEngine(QObject *parent, const QString &computeShaderPath) : QObject(parent) {
    surface = new QOffscreenSurface(nullptr, this);
    surface->create();
    surface->setFormat(QSurfaceFormat::defaultFormat());
    engineContext = QOpenGLContext::globalShareContext();

//    #ifdef DEBUG
    logger = new QOpenGLDebugLogger(this);
    connect(logger, &QOpenGLDebugLogger::messageLogged, [](auto msg) { qDebug() << msg.message().toUtf8(); });
//    #endif
    
    gl = engineContext->extraFunctions();
    camera = new Camera(this);
    textureManager = new TextureManager(this, engineContext);
    objectManager = new ObjectManager(this, engineContext, textureManager);
    lightManager = new LightManager(this, engineContext);
    
    textureManager->actualizeGLTextures();
    lightManager->actualizeGLLights();
    objectManager->actualizeGLObjects();
    
    computeProgram = new QOpenGLShaderProgram(engineContext);
    QOpenGLShader *computeShader = new QOpenGLShader(QOpenGLShader::ShaderTypeBit::Compute);
    computeShader->compileSourceFile(computeShaderPath);
    computeProgram->create();
    computeProgram->addShader(computeShader);
    computeProgram->link();
    delete computeShader;
    
    frameTimer = new QTimer(this);
    initRenderingCanvas();
}

TextureManager *GlassyEngine::getTextureManager() const {
    return textureManager;
}

ObjectManager *GlassyEngine::getObjectManager() const {
    return objectManager;
}

LightManager *GlassyEngine::getLightManager() const {
    return lightManager;
}

Camera *GlassyEngine::getCamera() const {
    return camera;
}

QOpenGLContext *GlassyEngine::getEngineContext() const {
    return engineContext;
}

void GlassyEngine::renderFrame() {
    engineContext->makeCurrent(surface);
    clock_t delta;
    if (realTimeRendering) {
        if (lastFrameInvocation == 0) {
            lastFrameInvocation = clock();
            delta = 0;
        } else {
            clock_t curTime = clock();
            delta = curTime - lastFrameInvocation;
            lastFrameInvocation = curTime;
        }
        emit frameRenderingStarted(delta);
        textureManager->actualizeGLTextures();
        lightManager->actualizeGLLights();
        objectManager->actualizeGLObjects();
    }
    
    computeProgram->bind();
    
    QVector3D ray00 = camera->getEyeRay(camera->getLeft(), camera->getBottom());
    QVector3D ray01 = camera->getEyeRay(camera->getLeft(), camera->getTop());
    QVector3D ray11 = camera->getEyeRay(camera->getRight(), camera->getTop());
    QVector3D ray10 = camera->getEyeRay(camera->getRight(), camera->getBottom());
    
    computeProgram->setUniformValue("eye", camera->getCamera());
    computeProgram->setUniformValue("ambient", (GLfloat) 0.1);
    computeProgram->setUniformValue("textureCount", textureManager->textureCount());
    computeProgram->setUniformValue("ray00", ray00);
    computeProgram->setUniformValue("ray01", ray01);
    computeProgram->setUniformValue("ray11", ray11);
    computeProgram->setUniformValue("ray10", ray10);
    
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    textureManager->bind();
    gl->glBindImageTexture(0, glRenderingCanvas, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    gl->glDispatchCompute(static_cast<GLuint>(qRound(resolution.width() / 32.0 + 1)),
                          static_cast<GLuint>(qRound(resolution.height() / 32.0 + 1)), 1);
    gl->glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    gl->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    textureManager->unbind();
    computeProgram->release();
    
    bindCanvas();
    emit frameRenderingDone();
    unbindCanvas();
    if (realTimeRendering) {
        frameTimer->singleShot(qMax(0l, timeForOneFrame - delta), this, &GlassyEngine::renderFrame);
    }
}

const QSize &GlassyEngine::getResolution() const {
    return resolution;
}

void GlassyEngine::setResolution(const QSize &resolution) {
    GlassyEngine::resolution = resolution;
    initRenderingCanvas();
    if (!realTimeRendering) {
        renderFrame();
    }
    camera->recompute_size(resolution.width(), resolution.height());
}

void GlassyEngine::initRenderingCanvas() {
    if (glRenderingCanvas != 0) {
        glDeleteTextures(1, &glRenderingCanvas);
    }
    glGenTextures(1, &glRenderingCanvas);
    glBindTexture(GL_TEXTURE_2D, glRenderingCanvas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.width(), resolution.height(), 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GlassyEngine::setFpsLock(quint32 fps) {
    fpsLock = true;
    maxFps = fps;
    timeForOneFrame = static_cast<clock_t>(qRound(1000.0 / fps));
}

void GlassyEngine::unlockFps() {
    fpsLock = false;
}

void GlassyEngine::bindCanvas() {
    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_2D, glRenderingCanvas);
}

void GlassyEngine::unbindCanvas() {
    gl->glBindTexture(GL_TEXTURE_2D, 0);
}



