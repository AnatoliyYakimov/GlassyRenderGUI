

#include <ObjectManagers/LightManager.h>

LightManager::LightManager(QObject *parent, QOpenGLContext *context) : QObject(parent), QOpenGLExtraFunctions(context) {
    pointLights = QSharedPointer<QVector<PointLight *>>::create();
    vectorLights = QSharedPointer<QVector<VectorLight *>>::create();
    lightsHash = QSharedPointer<QHash<QString, int>>::create();
}

void LightManager::actualizeGLLights() {
    if (shouldUpdateBuffer) {
        QVector<LightGL> lights(lightsHash->size());
        int counter = 0;
        lightsHash->clear();
            foreach (PointLight *pl, *pointLights) {
                lights.push_back(
                    LightGL{
                        pl->getColor(),
                        pl->getIntensity(),
                        pl->getPoint(),
                        pl->type
                    }
                );
                lightsHash->value(pl->getId(), counter++);
                pl->setUpdated();
            }
            foreach (VectorLight *vl, *vectorLights) {
                lights.push_back(
                    LightGL{
                        vl->getColor(),
                        vl->getIntensity(),
                        vl->getVector(),
                        vl->type
                    }
                );
                lightsHash->value(vl->getId(), counter++);
                vl->setUpdated();
            }
        if (glLightSSBO != 0) {
            glDeleteBuffers(1, &glLightSSBO);
        }
        glGenBuffers(1, &glLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glLightSSBO);
        if (lights.length() > 0) {
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightGL) * lights.length(), lights.data(),
                             GL_DYNAMIC_READ);
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, glLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        shouldUpdateBuffer = false;
    } else {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glLightSSBO);
            foreach (PointLight *pl, *pointLights) {
                if (pl->isNeedUpdate()) {
                    LightGL l = {
                        pl->getColor(),
                        pl->getIntensity(),
                        pl->getPoint(),
                        pl->type
                    };
                    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                                        lightsHash->value(pl->getId()),sizeof(LightGL), &l);
                    pl->setUpdated();
                }
            }
            foreach (VectorLight *vl, *vectorLights) {
                if (vl->isNeedUpdate()) {
                    LightGL l = {
                        vl->getColor(),
                        vl->getIntensity(),
                        vl->getVector(),
                        vl->type
                    };
                    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                                        lightsHash->value(vl->getId()),sizeof(LightGL), &l);
                    vl->setUpdated();
                }
            }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}

PointLight *LightManager::addPointLight(const QString &id, float intensity, QVector3D color, QVector3D point) {
    auto light = new PointLight(this, id, intensity, color, point);
    pointLights->push_back(light);
    return light;
}

VectorLight *LightManager::addVectorLight(const QString &id, float intensity, QVector3D color, QVector3D vector) {
    auto light = new VectorLight(this, id, intensity, color, vector);
    vectorLights->push_back(light);
    return light;
}

PointLight *LightManager::getPointLight(const QString &id) {
    for (auto it = pointLights->begin(); it != pointLights->end(); ++it) {
        if (id == (*it)->getId()) {
            return *it;
        }
    }
    return nullptr;
}

VectorLight *LightManager::getVectorLight(const QString &id) {
    for (auto it = vectorLights->begin(); it != vectorLights->end(); ++it) {
        if (id == (*it)->getId()) {
            return *it;
        }
    }
    return nullptr;
}
