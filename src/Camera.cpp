

#include <Camera.h>
#include <QtGui/QtGui>

void Camera::apply(const QMatrix4x4 &at) {
    camera = at * camera;
    direction = at * direction;
    direction.normalize();
    Vy = at * Vy;
    Vy.normalize();
    Vx = at * Vx;
    Vx.normalize();
    refreshViewMatrix();
    refreshInvViewProjectionMatrix();
}

Camera::Camera() {
}

void Camera::lookAt(QVector3D position, QVector3D lookAt, QVector3D up) {
//    v0 = camera;
//    v1 = (camera - lookAt).normalized();
//    Vx = QVector3D::crossProduct(v1, up.normalized()).normalized();
//    Vy = QVector3D::crossProduct(Vx, v1).normalized();
//    this->camera = v0;
//    direction = v1;
//    shouldRefreshViewMatrix = true;

    center = position;
    v1 = (lookAt - position).normalized();
    this->camera = position - FOV * v1;
    Vx = QVector3D::crossProduct(v1, up.normalized()).normalized();
    Vy = QVector3D::crossProduct(Vx, v1).normalized();
}

QVector3D Camera::getEyeRay(float x, float y) {
//    return at * (getInvViewProjectionMatrix() * QVector3D{x, y, 0} - camera);
    const QVector3D Pw = center + x * Vx + y * Vy;
    return Pw - camera;
}

void Camera::refreshViewMatrix() {
    v0 = direction.normalized();
    // right
    v1 = QVector3D::crossProduct(v0, Vy.normalized()).normalized();
    // up
    v2 = QVector3D::crossProduct(v1, v0).normalized();
    QVector4D row1 = QVector4D{v1, -1 * (QVector3D::dotProduct(v1, camera))};
    QVector4D row2 = QVector4D{v2, -1 * (QVector3D::dotProduct(v2, camera))};
    QVector4D row3 = QVector4D{v0, (QVector3D::dotProduct(v0, camera))};
    QVector4D row4 = QVector4D{0, 0, 0, 1};
    viewMatrix.setRow(0, row1);
    viewMatrix.setRow(1, row2);
    viewMatrix.setRow(2, row3);
    viewMatrix.setRow(3, row4);
    shouldRefreshViewMatrix = false;
    shouldRefreshInvViewProjectionMatrix = true;
}

void Camera::refreshProjectionMatrix() {
    float xDist = right - left;
    float yDist = top - bottom;
    float zDist = ffar - fnear;
    projectionMatrix = QMatrix4x4{
            2.0f * fnear / xDist, 0, -(right + left) / xDist, 0,
            0, 2.0f * fnear / yDist, -(top + bottom) / yDist, 0,
            0, 0, -(ffar + fnear) / zDist, -2.0f * ffar * fnear / zDist,
            0, 0, -1, 0
    };
    shouldRefreshProjectionMatrix = false;
    shouldRefreshInvViewProjectionMatrix = true;
    projectionMatrix = QMatrix4x4{
            2.0f / xDist, 0, 0, -1 * (right + left) / xDist,
            0, 2.0f / yDist, 0, -1 * (top + bottom) / yDist,
            0, 0, -2.0f / zDist, -1 * (ffar + fnear) / zDist,
            0, 0, 0, 1
    };
}

void Camera::refreshInvViewProjectionMatrix() {
    invViewProjectionMatrix = (getProjectionMatrix() * getViewMatrix()).inverted();
    shouldRefreshInvViewProjectionMatrix = false;
}

void Camera::setPerspective(float fovY, float aspect, float _near, float _far) {
    float w = (float) qTan(qDegreesToRadians(fovY) * 0.5f) * _near;
    float h = w / aspect;
    left = -w;
    right = left + 2.0f * w;
    bottom = -h;
    top = bottom + 2.0f * h;
    fnear = _near;
    ffar = _far;
    shouldRefreshProjectionMatrix = true;
}

const QVector3D &Camera::getDirection() const {
    return direction;
}

const QMatrix4x4 &Camera::getViewMatrix() {
    if (shouldRefreshViewMatrix) {
        refreshViewMatrix();
    }
    return viewMatrix;
}

const QMatrix4x4 &Camera::getProjectionMatrix() {
    if (shouldRefreshProjectionMatrix) {
        refreshProjectionMatrix();
    }
    return projectionMatrix;
}

const QMatrix4x4 &Camera::getInvViewProjectionMatrix() {
    if (shouldRefreshInvViewProjectionMatrix || shouldRefreshViewMatrix || shouldRefreshProjectionMatrix) {
        refreshInvViewProjectionMatrix();
    }
    return invViewProjectionMatrix;
}

const QVector3D &Camera::getPosition() const {
    return center;
}

void Camera::setVx(const QVector3D &vx) {
    Vx = vx;
    shouldRefreshViewMatrix = true;
}

void Camera::setVy(const QVector3D &vy) {
    Vy = vy;
    shouldRefreshViewMatrix = true;
}

void Camera::setDirection(const QVector3D &direction) {
    Camera::direction = direction;
    shouldRefreshViewMatrix = true;
}

void Camera::setPosition(const QVector3D &position) {
    Camera::camera = position;
    shouldRefreshViewMatrix = true;
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


