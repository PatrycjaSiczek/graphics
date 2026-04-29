#include "primitives.h"

Geometry *newAxesGeometry()
{
    Geometry* geometry = new Geometry();
    {
        geometry->setPrimitiveMode(GL_LINES);
        glm::vec3 verts[] = { {0,0,0}, {1.0,0,0}, {0,0,0}, {0,1.0,0}, {0,0,0}, {0,0,1.0} };
        glm::vec3 colors[] = { {1,0,0}, {1,0,0},  {0,1,0}, {0,1,0},   {0,0,1}, {0,0,1} };
        geometry->setVertices((int)Attributes::position, verts, 6);
        geometry->setAttribute((int)Attributes::color, colors, 6);
    }
    return geometry;
}

Geometry *newPlaneGeometry(glm::vec2 size, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLES);

    float hx = size.x / 2.0f;
    float hy = size.y / 2.0f;


    glm::vec3 verts[] = {
        {-hx, -hy, 0.0f},
        { hx, -hy, 0.0f},
        { hx,  hy, 0.0f},
        {-hx,  hy, 0.0f}
    };

    glm::vec3 colors[] = { color, color, color, color };


    uint indices[] = { 0, 1, 2, 0, 2, 3 };

    geometry->setVertices((uint)Attributes::position, verts, 4);
    geometry->setAttribute((uint)Attributes::color, colors, 4);
    geometry->setIndices(indices, 6);

    return geometry;
}

Geometry* newBoxGeometry(glm::vec3 size, glm::vec3 color) {
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLES);

    float w = size.x / 2.0f;
    float h = size.y / 2.0f;
    float d = size.z / 2.0f;


    glm::vec3 verts[] = {
        {-w,-h, d}, { w,-h, d}, { w, h, d}, {-w, h, d},
        {-w,-h,-d}, { w,-h,-d}, { w, h,-d}, {-w, h,-d}
    };

    glm::vec3 colors[8];
    for(int i=0; i<8; i++) colors[i] = color;

    uint indices[] = {
        0, 1, 2, 0, 2, 3,
        1, 5, 6, 1, 6, 2,
        5, 4, 7, 5, 7, 6,
        4, 0, 3, 4, 3, 7,
        3, 2, 6, 3, 6, 7,
        4, 5, 1, 4, 1, 0
    };

    geometry->setVertices((uint)Attributes::position, verts, 8);
    geometry->setAttribute((uint)Attributes::color, colors, 8);
    geometry->setIndices(indices, 36);

    return geometry;
}

Geometry* newCircleGeometry(float radius, int n_segments, glm::vec3 color) {
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLE_FAN);

    QVector<glm::vec3> verts;
    QVector<glm::vec3> colors;

    verts.push_back({0, 0, 0});
    colors.push_back(color);

    for(int i = 0; i <= n_segments; i++) {
        float angle = 2.0f * M_PI * float(i) / float(n_segments);
        verts.push_back({cos(angle) * radius, sin(angle) * radius, 0});
        colors.push_back(color);
    }

    geometry->setVertices((uint)Attributes::position, verts.data(), verts.size());
    geometry->setAttribute((uint)Attributes::color, colors.data(), colors.size());

    return geometry;
}
