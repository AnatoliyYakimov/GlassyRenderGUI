

#ifndef GLASSYRENDERGUI_GLASSYENGINE_H
#define GLASSYRENDERGUI_GLASSYENGINE_H


#include <QObject>
#include <QOpenGLContext>
#include <ObjectManagers/TextureManager.h>
#include <ObjectManagers/ObjectManager.h>
#include <ObjectManagers/LightManager.h>
#include <QtOpenGL/QGLShaderProgram>
#include <QOpenGLShader>
#include <QtOpenGL/QtOpenGL>
#include <QOffscreenSurface>
#include "Camera.h"

class GlassyEngine : public QObject {
Q_OBJECT
private:
    Camera *camera;
    TextureManager *textureManager;
    ObjectManager *objectManager;
    LightManager *lightManager;
    
    QOpenGLShaderProgram *computeProgram;
    QOpenGLContext *engineContext;
    QOffscreenSurface *surface;
    QOpenGLExtraFunctions *gl;
    QOpenGLDebugLogger *logger;
    GLuint glRenderingCanvas = 0;
    QSize resolution = QSize(1280, 720);
    
    QTimer *frameTimer;
    quint32 maxFps;
    bool fpsLock = false;
    bool realTimeRendering = false;
    clock_t timeForOneFrame;
    clock_t lastFrameInvocation = 0;
public:
    GlassyEngine(QObject *parent, const QString &computeShaderPath);
    
    void setFpsLock(quint32 fps);
    void unlockFps();
    
    void bindCanvas();
    void unbindCanvas();
signals:
    void frameRenderingStarted(long deltaTime);
    void frameRenderingDone();
    
public slots:
    
    TextureManager *getTextureManager() const;
    
    ObjectManager *getObjectManager() const;
    
    LightManager *getLightManager() const;
    
    Camera *getCamera() const;
    
    QOpenGLContext *getEngineContext() const;
    
    void renderFrame();
    
    const QSize &getResolution() const;
    
    void setResolution(const QSize &resolution);

private:
    void initRenderingCanvas();
    
};


#endif //GLASSYRENDERGUI_GLASSYENGINE_H
