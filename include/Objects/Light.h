

#ifndef GLASSYRENDERGUI_LIGHT_H
#define GLASSYRENDERGUI_LIGHT_H


#include <Objects/Texture.h>
#include <Util.h>

class Light : public QObject, public Updatable {
Q_OBJECT
private:
    QString id;
    float intensity;
    QVector3D color;
public:
    enum LightType {
        POINT = 0,
        VECTOR = 1
    };

    Light(QObject *parent, const QString &id, float intensity, const QVector3D &color)
            : QObject(parent), id(id),
              intensity(intensity),
              color(color) {}

    void setIntensity(float _intensity) {
        if (intensity != _intensity) {
            intensity = _intensity;
            setNeedToBeUpdated();
        }
    }

    void setColor(const QVector3D &_color) {
        if (color != _color) {
            color = _color;
            setNeedToBeUpdated();
        }
    }

    float getIntensity() const {
        return intensity;
    }

    const QVector3D &getColor() const {
        return color;
    }

    const QString &getId() {
        return id;
    }

    Q_PROPERTY(const QString &id
                       READ getId)
    Q_PROPERTY(float intensity
                       READ getIntensity
                       WRITE setIntensity)
    Q_PROPERTY(const QVector3D &color
                       READ getColor
                       WRITE setColor)
};

class PointLight : public Light {
Q_OBJECT
private:
    QVector3D point;
public:
    static const LightType type = LightType::POINT;

    PointLight(QObject *parent, const QString &id, float intensity, const QVector3D &color, const QVector3D &point)
            : Light(parent, id, intensity, color), point(point) {}

    const QVector3D &getPoint() const {
        return point;
    }

    void setPoint(const QVector3D &_point) {
        if (point != _point) {
            point = _point;
            setNeedToBeUpdated();
        }
    }

    Q_PROPERTY(const QVector3D &point
                       READ getPoint
                       WRITE setPoint)
};

class VectorLight : public Light {
Q_OBJECT
private:
    QVector3D vector;
public:
    static const LightType type = LightType::VECTOR;

    VectorLight(QObject *parent, const QString &id, float intensity, const QVector3D &color, const QVector3D &vector)
            : Light(parent, id, intensity, color), vector(vector) {}

    const QVector3D &getVector() const {
        return vector;
    }

    void setVector(const QVector3D &_vector) {
        if (vector != _vector) {
            vector = _vector;
            setNeedToBeUpdated();
        }
    }

    Q_PROPERTY(const QVector3D &vector
                       READ getVector
                       WRITE setVector)
};

#endif //GLASSYRENDERGUI_LIGHT_H
