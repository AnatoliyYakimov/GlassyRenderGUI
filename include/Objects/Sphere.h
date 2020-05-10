

#ifndef GLASSYRENDERGUI_SPHERE_H
#define GLASSYRENDERGUI_SPHERE_H

#include <QObject>
#include <QMatrix4x4>
#include <Util.h>

class Object : public QObject, public Updatable {
Q_OBJECT
private:
    const QString id;
    QString materialId;
    QMatrix4x4 invTransform;

public:
    Object(QObject *parent, const QString &id)
        : QObject(parent), id(id), invTransform() {}

    const QString &getId() const {
        return id;
    }

    const QString &getMaterialId() const {
        return materialId;
    }

    void setMaterialId(const QString &_materialId) {
        if (materialId != _materialId) {
            materialId == _materialId;
            setNeedToBeUpdated();
        }
    }

    const QMatrix4x4 &getInvTransform() const {
        return invTransform;
    }

    Q_PROPERTY(const QString &id READ getId)
    Q_PROPERTY(const QString &materialId WRITE setMaterialId READ getMaterialId)
    Q_PROPERTY(const QMatrix4x4& transform READ getInvTransform)

public slots:
    bool applyTransform(const QMatrix4x4 &tr) {
        bool res;
        invTransform *= tr.inverted(&res);
        return res;
    }
};

class Sphere : public Object {
Q_OBJECT
private:
    float radius;
    QVector3D center;
public:
    Sphere(QObject *parent, const QString &id, float radius, const QVector3D &center)
        : Object(parent, id),
          radius(radius),
          center(center) {}

    float getRadius() const {
        return radius;
    }

    void setRadius(float _radius) {
        if (radius != _radius) {
            radius = _radius;
            setNeedToBeUpdated();
        }
    }

    const QVector3D &getCenter() const {
        return center;
    }

    void setCenter(const QVector3D &_center) {
        if (center != _center) {
            center = _center;
            setNeedToBeUpdated();
        }
    }

    Q_PROPERTY(const QVector3D &center READ getCenter WRITE setCenter)
    Q_PROPERTY(float radius READ getRadius WRITE setRadius)
};


#endif //GLASSYRENDERGUI_SPHERE_H
