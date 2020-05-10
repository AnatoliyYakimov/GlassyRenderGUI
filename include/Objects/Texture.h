

#ifndef GLASSYRENDERGUI_TEXTURE_H
#define GLASSYRENDERGUI_TEXTURE_H

#include <QObject>
#include <QVector3D>
#include <QImage>
#include <Util.h>

class Texture : public QObject, public Updatable {
Q_OBJECT
private:
    const QString id;
public:
    Texture(QObject *parent, const QString &id) : QObject(parent), id(id) {}

    const QString &getId() const {
        return id;
    }

    Q_PROPERTY(const QString& id
                       READ
                       getId)
};

class UniformTexture : public Texture {
Q_OBJECT
private:
    QVector3D value;
public:

    UniformTexture(QObject *parent, const QString &id, const QVector3D &value)
            : Texture(parent, id), value(value) {}

    const QVector3D &getValue() const {
        return value;
    }

    void setValue(const QVector3D &_value) {
        if (value != _value) {
            value = _value;
            setNeedToBeUpdated();
        }
    }

    Q_PROPERTY(const QVector3D &value
                       READ
                       getValue
                       WRITE
                       setValue)
};

class TextureMap : public QObject, public Updatable {
Q_OBJECT
private:
    const QString id;
    QImage *image;
public:
    TextureMap(QObject *parent, const QString &id, QImage &&image)
            : QObject(parent), id(id), image(new QImage(image)) {}

    const QString &getId() const {
        return id;
    }

    QImage *getImage() {
        return image;
    }

    void setImage(QImage *_image) {
        if (image != _image) {
            image = _image;
            setNeedToBeUpdated();
        }
    }

    Q_PROPERTY(const QString& id
                       READ
                       getId)
    Q_PROPERTY(QImage *image
                       READ
                               getImage
                       WRITE
                       setImage)

    virtual ~TextureMap() {
        delete image;
    }
};

class MappedTexture : public Texture {
Q_OBJECT
private:
    TextureMap *map;
public:
    MappedTexture(QObject *parent, const QString &id, TextureMap *_map)
            : Texture(parent, id),
              map(_map) {}


    TextureMap *getTextureMap() const {
        return map;
    }

    void setTextureMap(TextureMap *textureMap) {
        if (map != textureMap) {
            textureMap = textureMap;
            setNeedToBeUpdated();
        }
    }

    Q_PROPERTY(TextureMap *textureMap
                       READ
                       getTextureMap
                       WRITE
                       setTextureMap)
};

class Material : public QObject, public Updatable {
Q_OBJECT
private:
    const QString id;
    QString albedo;
    QString roughness;
    QString ao;
    QString normal;
public:
    Material(QObject *parent,
             const QString &id,
             const QString &albedo,
             const QString &roughness,
             const QString &ao,
             const QString &normal) : QObject(parent), id(id), albedo(albedo), roughness(roughness), ao(ao),
                                      normal(normal) {}

    const QString &getId() const {
        return id;
    }

    QString getAlbedo() const {
        return albedo;
    }

    void setAlbedo(QString albedo) {
        if (this->albedo != albedo) {
            this->albedo = albedo;
            setNeedToBeUpdated();
        }
    }

    QString getRoughness() const {
        return roughness;
    }

    void setRoughness(QString roughness) {
        if (this->roughness != roughness) {
            this->roughness = roughness;
            setNeedToBeUpdated();
        }
    }

    QString getAo() const {
        return ao;
    }

    void setAo(QString ao) {
        if (this->ao != ao) {
            this->ao = ao;
            setNeedToBeUpdated();
        }
    }

    QString getNormal() const {
        return normal;
    }

    void setNormal(QString normal) {
        if (this->normal != normal) {
            this->normal = normal;
            setNeedToBeUpdated();
        }
    }

    Q_PROPERTY(QString id
                       READ
                       getId)
    Q_PROPERTY(QString albedo
                       READ
                               getAlbedo
                       WRITE
                       setAlbedo)
    Q_PROPERTY(QString roughness
                       READ
                               getRoughness
                       WRITE
                       setRoughness)
    Q_PROPERTY(QString ao
                       READ
                               getAo
                       WRITE
                       setRoughness)
    Q_PROPERTY(QString normal
                       READ
                               getNormal
                       WRITE
                       setNormal)
};


#endif //GLASSYRENDERGUI_TEXTURE_H
