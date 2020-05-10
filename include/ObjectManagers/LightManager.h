

#ifndef GLASSYRENDERGUI_LIGHTMANAGER_H
#define GLASSYRENDERGUI_LIGHTMANAGER_H

#include <GL/gl.h>
#include <QHash>
#include <QVector>
#include <QSharedPointer>
#include <Objects/Light.h>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

struct LightGL {
    QVector3D color;
    GLfloat intensity;
    QVector3D value;
    GLuint type;
};

class LightManager : public QObject, protected QOpenGLExtraFunctions {
Q_OBJECT
private:
    GLuint glLightSSBO = 0;
    QSharedPointer<QVector<PointLight *>> pointLights;
    QSharedPointer<QVector<VectorLight *>> vectorLights;
    QSharedPointer<QHash<QString, int>> lightsHash;

    bool shouldUpdateBuffer = true;

public:
    LightManager(QObject *parent, QOpenGLContext* context);

    void actualizeGLLights();

public slots:
    PointLight *addPointLight(const QString &id, float intensity, QVector3D color, QVector3D point);

    VectorLight *addVectorLight(const QString &id, float intensity, QVector3D color, QVector3D vector);

    PointLight *getPointLight(const QString &id);

    VectorLight *getVectorLight(const QString &id);

};


#endif //GLASSYRENDERGUI_LIGHTMANAGER_H
