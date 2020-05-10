

#include <Camera.h>
void Camera::lookAt(QVector3D position, QVector3D lookAt, QVector3D up) {
    center = position;
    v1 = (lookAt - position).normalized();
    float d = center.distanceToPoint(right * Vx + center);
    this->camera = position - cameraDistance / d * v1;
    Vx = QVector3D::crossProduct(v1, up.normalized()).normalized();
    Vy = QVector3D::crossProduct(Vx, v1).normalized();
}

QVector3D Camera::getEyeRay(float x, float y) {
    const QVector3D Pw = center + x * Vx + y * Vy;
    return (Pw - camera).normalized();
}

const QVector3D &Camera::getDirection() const {
    return direction;
}

const QVector3D &Camera::getPosition() const {
    return center;
}

float Camera::getLeft() const {
    return left;
}

float Camera::getTop() const {
    return top;
}

float Camera::getBottom() const {
    return bottom;
}

float Camera::getRight() const {
    return right;
}

const QVector3D &Camera::getCamera() const {
    return camera;
}

void Camera::recompute_size(int width, int height) {
    float d = static_cast<float>((right - left) * height / (width * 2.0));
    float middle = bottom + (top - bottom) / 2;
    bottom = middle - d;
    top = middle + d;
}

void Camera::setFov(float FOV) {
    fov = FOV;
    cameraDistance = static_cast<float>(abs(qTan(fov / 2.0)));
}

float Camera::getFov() const {
    return fov;
}

Camera::Camera(QObject *parent) : QObject(parent) {}


