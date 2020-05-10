

#ifndef GLASSYRENDERGUI_TEXTUREMANAGER_H
#define GLASSYRENDERGUI_TEXTUREMANAGER_H

#include <Objects/Texture.h>
#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <GL/gl.h>
#include <QtGui/QImage>
#include <QtGui/QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QtGui/QOpenGLDebugLogger>
#include <QHash>

struct MaterialGL {
    GLuint alb;
    GLuint rgh;
    GLuint ao;
    GLuint nrm;
};

struct TextureMetaGL {
    QVector3D value;
    GLuint type;
};

enum TextureTypeGL {
    MAPPED = 0,
    UNIFORM = 1
};

class TextureManager : public QObject, protected QOpenGLExtraFunctions {
Q_OBJECT
private:
    const uint width = 2048;
    const uint height = 2048;
    GLuint glTextureMaps = 0;
    GLuint glTextureMetaSSBO = 0;
    GLuint glMaterialSSBO = 0;
    
    QOpenGLExtraFunctions* gl;
    
    QSharedPointer<QVector<UniformTexture *>> uniformTextures;
    QSharedPointer<QVector<MappedTexture *>> mappedTextures;
    QSharedPointer<QVector<TextureMap *>> textureMaps;
    QSharedPointer<QVector<Material *>> materials;

    QSharedPointer<QHash<QString, int>> metaHash;
    QSharedPointer<QHash<QString, int>> materialsHash;
    QSharedPointer<QHash<QString, int>> textureMapHash;


    bool shouldUpdateTextureMeta = true;
    bool shouldUpdateTextureMaps = true;
    bool shouldUpdateTextureMaterials = true;

public:

    TextureManager(QObject *parent, QOpenGLContext* context);

    void actualizeGLTextures();
    
    GLuint textureCount();
    
    void bind();
    
    void unbind();

    int getMaterialGLPosition(const QString &id) const;

public slots:

    UniformTexture *addUniformTexture(const QString &id, QVector3D value);

    MappedTexture *addMappedTexture(const QString &id, TextureMap *textureMap);

    TextureMap *addTextureMap(const QString &id, const QString &path, const QString &format);

    Material *
    addMaterial(const QString &id, const QString &albedo, const QString &rgh, const QString &ao, const QString &nrm);

    UniformTexture *getUniformTexture(const QString &id);

    MappedTexture *getMappedTexture(const QString &id);

    TextureMap *getTextureMap(const QString &id);

};

#endif //GLASSYRENDERGUI_TEXTUREMANAGER_H
