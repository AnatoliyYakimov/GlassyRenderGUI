

#ifndef GLASSYRENDERGUI_OBJECTMANAGER_H
#define GLASSYRENDERGUI_OBJECTMANAGER_H


#include <ObjectManagers/TextureManager.h>
#include <objects/Sphere.h>

struct SphereGL {
    QVector3D center;
    GLfloat radius;
    GLuint material;
    GLfloat padding[3];
};

class ObjectManager : public QObject, protected QOpenGLExtraFunctions {
private:
    GLuint glSphereSSBO = 0;
    QSharedPointer<QHash<QString, Sphere *>> spheres;
    QSharedPointer<QHash<QString, int>> sphereHash;
    const TextureManager *textureManager;

    bool shouldUpdateSpheres = true;
public:
    ObjectManager(QObject *parent, QOpenGLContext *context, const TextureManager *tm);

    void actualizeGLObjects();

public slots:

    Sphere *addSphere(const QString &id, float radius, const QVector3D &center, const QString &materialId);

    Sphere *getSphere(const QString &id);

};


#endif //GLASSYRENDERGUI_OBJECTMANAGER_H
