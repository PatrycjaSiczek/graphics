#include "geometry.h"

Geometry::Geometry()
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &vao);

    primitiveMode = GL_TRIANGLES;
    n_verts = 0;
    n_indices = 0;

    ibo = 0;
}

Geometry::~Geometry() {
    glDeleteVertexArrays(1, &vao);
    for(GLuint buffer : bufferObjects) {
        glDeleteBuffers(1, &buffer);
    }
    if(ibo != 0) glDeleteBuffers(1, &ibo);
}


void Geometry::render() {
    glBindVertexArray(vao);
    if(ibo != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(primitiveMode, n_indices, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(primitiveMode, 0, n_verts);
    }
    glBindVertexArray(0);
}

void Geometry::setVertices(uint index, glm::vec3* verts, int n)
{
    n_verts = n;
    setAttribute(index, verts, n);
}

void Geometry::setIndices(uint *data, int n) {
    n_indices = n;
    if(ibo == 0) glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(uint), data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Geometry::setPrimitiveMode(GLenum mode)
{
    primitiveMode = mode;
}

void Geometry::setAttribute(uint index, glm::vec3 *data, int n) {
    if(!bufferObjects.contains(index)) {
        GLuint buffer;
        glGenBuffers(1, &buffer);
            bufferObjects.insert(index, buffer);
    }

    glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[index]);
        glBufferData(GL_ARRAY_BUFFER, n * sizeof(glm::vec3), data, GL_STATIC_DRAW);

        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

