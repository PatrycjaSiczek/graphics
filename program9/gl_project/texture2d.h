#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <QOpenGLFunctions_4_1_Core>
#include "utils.h"
#include "glm/glm.hpp"

class Texture2D : protected QOpenGLFunctions_4_1_Core
{
    GLuint handle;
public:
    Texture2D();
    bool loadFromFile(const char* file);
    void bind(int tex_unit);
};

#endif // TEXTURE2D_H
