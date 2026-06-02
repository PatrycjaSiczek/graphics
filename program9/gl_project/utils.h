#ifndef UTILS_H
#define UTILS_H

#include <QDebug>

#include <GL/gl.h>
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <QOpenGLFunctions_4_2_Core>
#include <QMap>
#include "utils.h"
#include "glm/glm.hpp"

enum class Attributes{
    position = 0,
    color = 1,
    normal = 2,
    uv1 = 7,
};

struct Frame
{
    glm::vec3 pos;
    glm::vec3 up;
    glm::vec3 forward;

    Frame() {
        pos = glm::vec3(0,0,0);
        up = glm::vec3(0,1,0);
        forward = glm::vec3(0,0,1);
    }
    glm::vec3 s() { return glm::normalize(glm::cross(up, forward)); }

    glm::mat4 matrix(){
        glm::mat4 m(1.0f);
        m[0] = glm::vec4(s(), 0.0f);
        m[1] = glm::vec4(up, 0.0f);
        m[2] = glm::vec4(forward, 0.0f);
        m[3] = glm::vec4(pos, 1.0f);
        return m;
    }
};

struct Camera
{
    glm::vec3 pos;
    glm::vec3 up;
    glm::vec3 forward;

    Camera() {
        pos = glm::vec3(0,0,0);
        up = glm::vec3(0,1,0);
        forward = glm::vec3(0,0,-1);
    }

    glm::vec3 s() { return glm::normalize(glm::cross(forward, up)); }

    glm::mat4 matrix(){
        glm::mat4 m(1.0f);
        glm::vec3 xx = s();
        glm::vec3 f = glm::normalize(forward);
        glm::vec3 u = glm::normalize(glm::cross(xx, forward));
        m[0] = glm::vec4(xx, 0.0f);
        m[1] = glm::vec4(u, 0.0f);
        m[2] = glm::vec4(-f, 0.0f);
        m = glm::transpose(m);

        glm::mat4 tr = glm::translate(glm::mat4(1.0f), glm::vec3(-pos.x, -pos.y, -pos.z));
        m = m * tr;
        return m;
    }
};

#endif // UTILS_H
