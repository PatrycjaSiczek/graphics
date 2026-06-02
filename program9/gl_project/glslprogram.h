#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H


#include <QOpenGLFunctions_4_2_Core>
#include <string>
#include <QDebug>

#include "glm/glm.hpp"
#include "utils.h"


class GLSLProgram :  protected QOpenGLFunctions_4_2_Core
{
    GLuint handle;
    bool checkShaderStatus(GLuint);
    bool checkProgramStatus(GLuint);

public:
    GLSLProgram();
    ~GLSLProgram();
    bool compileShaderFromFile(const char* filename, GLenum type);
    bool compileShaderFromString(const std::string& source, GLenum type);
    bool link();
    void use();    

    void setUniform(const char* name, glm::vec3 v);
    void setUniform(const char* name, glm::mat4 mat);
    void setUniform(const char* name, float v);

    void setUniform(const char* name, int v);
    void setUniform(const char* name, glm::mat3 mat);
    void setUniform(const char* name, glm::vec2 v);
    //void bindAttribLocation(GLuint location, const char* name);
};

#endif // GLSLPROGRAM_H
