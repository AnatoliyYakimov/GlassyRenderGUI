

#ifndef GLASSYRENDERGUI_CAMERA_H
#define GLASSYRENDERGUI_CAMERA_H

#include <QtGui/QMatrix4x4>
#include <QtGui/QVector2D>
#include <QtCore/QObject>
#include <QtGui>

class Camera : public QObject {
Q_OBJECT
private:
    QVector3D Vx{1, 0, 0};
    QVector3D Vy{0, 1, 0};

    QVector3D direction;
    QVector3D center;
    QVector3D camera;
    QVector3D v1;

    float left = -1;
    float top = 1;
    float bottom = -1;
    float right = 1;
    float cameraDistance = 0;
    float fov;
public:
    Camera(QObject *parent);

    QVector3D getEyeRay(float x, float y);
    void lookAt(QVector3D position, QVector3D lookAt, QVector3D up);
    void recompute_size(int width, int height);

    void setFov(float FOV);
    float getFov() const;

    const QVector3D &getCamera() const;
    const QVector3D &getDirection() const;
    const QVector3D &getPosition() const;

    float getLeft() const;
    float getTop() const;
    float getBottom() const;
    float getRight() const;

    Q_PROPERTY(float fov READ getFov WRITE setFov)
};

#endif //GLASSYRENDERGUI_CAMERA_H
