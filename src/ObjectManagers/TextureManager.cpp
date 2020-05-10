#include <ObjectManagers/TextureManager.h>

TextureManager::TextureManager(QObject *parent, QOpenGLContext *context) : QObject(parent),
                                                                           QOpenGLExtraFunctions(context) {
    uniformTextures = uniformTextures.create();
    mappedTextures = mappedTextures.create();
    textureMapHash = textureMapHash.create();
    materials = materials.create();
    materialsHash = materialsHash.create();
    textureMaps = textureMaps.create();
    metaHash = metaHash.create();

}

UniformTexture *TextureManager::addUniformTexture(const QString &id, QVector3D value) {
    auto tex = new UniformTexture(this, id, value);
    uniformTextures->push_back(tex);
    shouldUpdateTextureMeta = true;
    return tex;
}

MappedTexture *TextureManager::addMappedTexture(const QString &id, TextureMap *textureMap) {
    auto tex = new MappedTexture(this, id, textureMap);
    mappedTextures->push_back(tex);
    shouldUpdateTextureMeta = true;
    return tex;
}

TextureMap *TextureManager::addTextureMap(const QString &id, const QString &path, const QString &format) {
    QImage *img = new QImage(2048, 2048, QImage::Format::Format_RGBA8888);
    img->load(path, format.toStdString().data());
    textureMapHash->value(id, textureMaps->length());
    auto map = new TextureMap(this, id, qMove(*img));
    textureMaps->push_back(map);
    shouldUpdateTextureMaps = true;
    delete img;
    return map;
}

Material *TextureManager::addMaterial(const QString &id,
                                      const QString &albedo,
                                      const QString &rgh,
                                      const QString &ao,
                                      const QString &nrm) {
    auto mat = new Material(this, id, albedo, rgh, ao, nrm);
    materialsHash->value(id, materials->length());
    materials->push_back(mat);
    return mat;
}

UniformTexture *TextureManager::getUniformTexture(const QString &id) {
    QVectorIterator<UniformTexture *> it(*uniformTextures);
    while (it.hasNext()) {
        auto t = it.next();
        if (t->getId() == id) {
            return t;
        }
    }
    return nullptr;
}

MappedTexture *TextureManager::getMappedTexture(const QString &id) {
    QVectorIterator<MappedTexture *> it(*mappedTextures);
    while (it.hasNext()) {
        auto t = it.next();
        if (t->getId() == id) {
            return t;
        }
    }
    return nullptr;
}

TextureMap *TextureManager::getTextureMap(const QString &id) {
    QVectorIterator<TextureMap *> it(*textureMaps);
    while (it.hasNext()) {
        auto t = it.next();
        if (t->getId() == id) {
            return t;
        }
    }
    return nullptr;
}

void TextureManager::actualizeGLTextures() {
    //Обновляем текстуры
    if (shouldUpdateTextureMaps) {
        if (glTextureMaps != 0) {
            glDeleteTextures(1, &glTextureMaps);
        }
        int len = textureMaps->length();

        glGenTextures(1, &glTextureMaps);
        glBindTexture(GL_TEXTURE_2D_ARRAY, glTextureMaps);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, width, height, len);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, width, height, 6, 0, GL_BGRA,
                         GL_UNSIGNED_INT_8_8_8_8_REV, nullptr);
        for (int i = 0; i < len; ++i) {
            auto tex = textureMaps->at(i);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1,
                                GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, tex->getImage()->bits());
            tex->setUpdated();
        }
        shouldUpdateTextureMaps = false;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D_ARRAY, glTextureMaps);
        int len = textureMaps->length();
        for (int i = 0; i < len; ++i) {
            auto tex = textureMaps->value(i);
            if (tex->isNeedUpdate()) {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1,
                                    GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, tex->getImage()->bits());
                tex->setUpdated();
            }
        }
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
    
    if (shouldUpdateTextureMeta) {
        int uniformCount = uniformTextures->length();
        int mappedCount = mappedTextures->length();
        QVector<TextureMetaGL> meta(uniformCount + mappedCount);
        for (int i = 0; i < uniformCount; ++i) {
            auto tex = uniformTextures->at(i);
            metaHash->value(tex->getId(), i);
            meta.push_back(
                TextureMetaGL{
                    tex->getValue(),
                    TextureTypeGL::UNIFORM
                }
            );
        }
        for (int j = 0; j < mappedCount; ++j) {
            auto tex = mappedTextures->at(j);
            metaHash->value(tex->getId(), uniformCount + j);
            meta.push_back(TextureMetaGL{
                               QVector3D{
                                   static_cast<GLfloat>(textureMapHash->value(tex->getTextureMap()->getId())),
                                   0,
                                   0
                               },
                               TextureTypeGL::MAPPED
                           }
            );
        }
        if (glTextureMetaSSBO != 0) {
            glDeleteBuffers(1, &glTextureMetaSSBO);
        }
        glGenBuffers(1, &glTextureMetaSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glTextureMetaSSBO);
        if (meta.length() > 0) {
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TextureMetaGL) * meta.length(), meta.data(), GL_DYNAMIC_READ);
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, glTextureMetaSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    } else {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glTextureMetaSSBO);
        int uniformCount = uniformTextures->length();
        int mappedCount = mappedTextures->length();
        for (int i = 0; i < uniformCount + mappedCount; ++i) {
            auto tex = uniformTextures->at(i);
            if (tex->isNeedUpdate()) {
                auto meta = TextureMetaGL{
                    tex->getValue(),
                    TextureTypeGL::UNIFORM
                };
                int position = metaHash->value(tex->getId());
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, position, sizeof(TextureMetaGL), &meta);
                tex->setUpdated();
            }
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    
    if (shouldUpdateTextureMaterials) {
        if (glMaterialSSBO != 0) {
            glDeleteBuffers(1, &glMaterialSSBO);
        }
        glGenBuffers(1, &glMaterialSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glMaterialSSBO);
        QVector<MaterialGL> materialsGL(materials->length());
        for (auto it = materials->begin(); it != materials->end(); it++) {
            auto mat = *it;
            materialsGL.push_back(
                MaterialGL{
                    static_cast<GLuint>(metaHash->value(mat->getAlbedo())),
                    static_cast<GLuint>(metaHash->value(mat->getRoughness())),
                    static_cast<GLuint>(metaHash->value(mat->getAo())),
                    static_cast<GLuint>(metaHash->value(mat->getNormal()))
                }
            );
        }
        if (materialsGL.length() > 0) {
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(MaterialGL) * materialsGL.length(), materialsGL.data(),
                         GL_DYNAMIC_READ);
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, glMaterialSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        shouldUpdateTextureMaterials = false;
    } else {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, glMaterialSSBO);
        for (int i = 0; i < materials->length(); ++i) {
            auto mat = materials->at(i);
            auto matGl = MaterialGL{
                static_cast<GLuint>(metaHash->value(mat->getAlbedo())),
                static_cast<GLuint>(metaHash->value(mat->getRoughness())),
                static_cast<GLuint>(metaHash->value(mat->getAo())),
                static_cast<GLuint>(metaHash->value(mat->getNormal()))
            };
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, i, sizeof(MaterialGL), &matGl);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}

int TextureManager::getMaterialGLPosition(const QString &id) const {
    return materialsHash->value(id);
}

void TextureManager::bind() {
    glBindTexture(GL_TEXTURE_2D_ARRAY, glTextureMaps);
}

void TextureManager::unbind() {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

GLuint TextureManager::textureCount() {
    return static_cast<GLuint>(mappedTextures->length());
}


