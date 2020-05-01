

#ifndef GLASSYRENDERGUI_CAMERA_H
#define GLASSYRENDERGUI_CAMERA_H

#include <QtGui/QMatrix4x4>
#include <QtGui/QVector2D>

class Camera {
private:
    QVector3D Vx{1, 0, 0};
    QVector3D Vy{0, 1, 0};

    QVector3D direction;
    QVector3D center;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 invViewProjectionMatrix;
    QVector3D v0;
    QVector3D v1;
    QVector3D v2;

    float left = -1;
    float top = 1;
    float bottom = -1;
    float right = 1;
    float fnear = 0.001;
    float ffar = 10;

    bool shouldRefreshViewMatrix = true;
    bool shouldRefreshProjectionMatrix = true;
    bool shouldRefreshInvViewProjectionMatrix = true;
public:
    QVector3D camera;
private:
    void refreshViewMatrix();
    void refreshProjectionMatrix();
    void refreshInvViewProjectionMatrix();

    float FOV = 1.3f;

    inline QVector2D camera_to_viewport(int u, int v) {
        float x = left + u * (right - left);
        float y = top - v * (top - bottom);
        return QVector2D{x, y};
    }


public:
    Camera();
    void apply(const QMatrix4x4 &at);
    void setPerspective(float fovY, float aspect, float _near, float _far);

    QVector3D getEyeRay(float x, float y);
    void lookAt(QVector3D position, QVector3D lookAt, QVector3D up);

    void recompute_size(int width, int height) {
        float d = (right - left) * height / width / 2;
        float middle = bottom + (top - bottom) / 2;
        bottom = middle - d;
        top = middle + d;
    }

    inline QVector3D screen_to_world(const int &u, const int &v) {
        const QVector2D Pv = camera_to_viewport(u, v);
        const QVector3D Pw = camera + Pv[0] * Vx + Pv[1] * Vy;
        return Pw - camera;
    };

    const QVector3D &getDirection() const;

    const QMatrix4x4 &getViewMatrix();

    const QMatrix4x4 &getProjectionMatrix();

    const QMatrix4x4 &getInvViewProjectionMatrix();

    const QVector3D &getPosition() const;

    void setVx(const QVector3D &vx);

    void setVy(const QVector3D &vy);

    void setDirection(const QVector3D &direction);

    void setPosition(const QVector3D &position);

    float getLeft() const;

    float getTop() const;

    float getBottom() const;

    float getRight() const;
};
#endif //GLASSYRENDERGUI_CAMERA_H
