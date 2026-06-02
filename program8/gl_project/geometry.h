#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QOpenGLFunctions_4_2_Core>
#include <QMap>
#include "utils.h"
#include "glm/glm.hpp"

class Geometry : protected QOpenGLFunctions_4_2_Core
{
protected:
    GLuint vao;
    int n_verts;

    GLuint ibo;
    int n_indices;


    QMap<uint, GLuint> bufferObjects;
    GLenum primitiveMode;
public:        
    Geometry();
    virtual ~Geometry();

    void setPrimitiveMode(GLenum);
    virtual void render();

    void setIndices(uint* data, int n);

    void setVertices(uint index, glm::vec3* verts, int n);
    void setAttribute(uint index, glm::vec3* data, int n);


    void setAttribute(uint index, glm::vec2* data, int n);
    void setAttribute(uint index, float* data, int n);
};




#endif // GEOMETRY_H
