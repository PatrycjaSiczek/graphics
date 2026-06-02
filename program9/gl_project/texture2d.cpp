#include "texture2d.h"
#include <QImage>

Texture2D::Texture2D() {
    initializeOpenGLFunctions();

    glGenTextures(1, &handle);
}

bool Texture2D::loadFromFile(const char *file) {
    QImage image(file);
    if (!image.isNull()) {
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA,
                     image.width(), image.height(),
                     0,
                     GL_BGRA,
                     GL_UNSIGNED_BYTE,
                     image.bits()
                     );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        qDebug() << "Nie wczytana tekstura";
        return false;
    }
    return true;
}

void Texture2D::bind(int tex_unit) {
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D, handle);
}
