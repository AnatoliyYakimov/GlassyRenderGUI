

#include <ObjectManagers/ObjectManager.h>

ObjectManager::ObjectManager(QObject *parent, QOpenGLContext *context, const TextureManager *tm)
    : QObject(parent), QOpenGLExtraFunctions(context), textureManager(tm) {
    sphereHash = QSharedPointer<QHash<QString, int>>::create();
    spheres = QSharedPointer<QHash<QString, Sphere *>>::create();
}

void ObjectManager::actualizeGLObjects() {
    if (shouldUpdateSpheres) {
        if (glSphereSSBO != 0) {
            glDeleteBuffers(1, &glSphereSSBO);
        }
        glGenBuffers(1, &glSphereSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glSphereSSBO);
        int counter = 0;
        sphereHash->clear();
        QVector<SphereGL> spheresGL;
            foreach (Sphere *s, *spheres) {
                int materialIdx = textureManager->getMaterialGLPosition(s->getMaterialId());
                spheresGL.push_back(
                    SphereGL{
                        s->getCenter(),
                        s->getRadius(),
                        static_cast<GLuint>(materialIdx)
                    });
                sphereHash->value(s->getId(), counter++);
                s->setUpdated();
            }
        if (spheresGL.length() > 0) {
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SphereGL) * counter, spheresGL.data(),
                         GL_DYNAMIC_READ);
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, glSphereSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        shouldUpdateSpheres = false;
    } else {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glSphereSSBO);
            foreach (Sphere *s, *spheres) {
                if (!s->isNeedUpdate())
                    continue;
                int materialIdx = textureManager->getMaterialGLPosition(s->getMaterialId());
                auto sphere = SphereGL{
                    s->getCenter(),
                    s->getRadius(),
                    static_cast<GLuint>(materialIdx)
                };
                int idx = sphereHash->value(s->getId());
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, idx, sizeof(SphereGL), &sphere);
                s->setUpdated();
            }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}

Sphere *ObjectManager::addSphere(const QString &id, float radius, const QVector3D &center, const QString &materialId) {
    auto sphere = new Sphere(this, id, radius, center);
    if (spheres->contains(id)) {
        delete spheres->take(id);
    }
    spheres->value(id, sphere);
    shouldUpdateSpheres = true;
    return sphere;
}

Sphere *ObjectManager::getSphere(const QString &id) {
    return spheres->value(id);
}
