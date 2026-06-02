#include "primitives.h"
#include "glslprogram.h"
#include "utils.h"

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
    {
        geometry->setPrimitiveMode(GL_TRIANGLES);
        glm::vec3 verts[]  = { {-size.x,-size.y,0}, {-size.x,size.y,0}, {size.x,size.y,0}, {size.x,-size.y,0}};
        glm::vec3 colors[] = { color, color, color, color};
        glm::vec3 n = {0,0,1};
        glm::vec3 normals[] = {n,n,n,n};
        glm::vec2 uvs[] = {{0,0}, {0,1}, {1,1}, {1,0}};
        uint indices[] = {0,1,2,2,3,0};
        geometry->setIndices(indices, 6);
        geometry->setVertices((int)Attributes::position, verts, 4);
        geometry->setAttribute((int)Attributes::color, colors, 4);
        geometry->setAttribute((int)Attributes::normal, normals, 4);
    }
    return geometry;
}

Geometry* newBoxGeometry(glm::vec3 size, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLES);
    {
        int n = 24;
        glm::vec3 origin = {0.0, 0.0, 0.0};
        float w = size.x;
        float h = size.y;
        float d = size.z;

        std::vector<glm::vec3> pos(n);
        std::vector<glm::vec3> colors(n);
        std::vector<uint> indices(36);

        int it=0;
        // sciana z = +1  //front
        pos[it++] = origin + glm::vec3{-w,  h, d};
        pos[it++] = origin + glm::vec3{-w, -h, d};
        pos[it++] = origin + glm::vec3{ w, -h, d};
        pos[it++] = origin + glm::vec3{ w,  h, d};

        // sciana x=+1   //right
        pos[it++] = origin + glm::vec3{w,  h, d};
        pos[it++] = origin + glm::vec3{w, -h, d};
        pos[it++] = origin + glm::vec3{w, -h, -d};
        pos[it++] = origin + glm::vec3{w,  h, -d};

        // sciana z=-1   //back
        pos[it++] = origin + glm::vec3{-w,  h, -d};
        pos[it++] = origin + glm::vec3{-w, -h, -d};
        pos[it++] = origin + glm::vec3{ w, -h, -d};
        pos[it++] = origin + glm::vec3{ w,  h, -d};

        // sciana x=-1   //left
        pos[it++] = origin + glm::vec3{-w,  h, d};
        pos[it++] = origin + glm::vec3{-w, -h, d};
        pos[it++] = origin + glm::vec3{-w, -h, -d};
        pos[it++] = origin + glm::vec3{-w,  h, -d};

        // sciana y=1  //top
        pos[it++] = origin + glm::vec3{-w, h, -d};
        pos[it++] = origin + glm::vec3{-w, h,  d};
        pos[it++] = origin + glm::vec3{ w, h,  d};
        pos[it++] = origin + glm::vec3{ w, h, -d};

        // sciana y=-1  //bottom
        pos[it++] = origin + glm::vec3{-w, -h, -d};
        pos[it++] = origin + glm::vec3{-w, -h,  d};
        pos[it++] = origin + glm::vec3{ w, -h,  d};
        pos[it++] = origin + glm::vec3{ w, -h, -d};

        for(int i=0; i<n; i++)
            colors[i] = color;
        geometry->setVertices((int)Attributes::position, pos.data(), n);
        geometry->setAttribute((int)Attributes::color, colors.data(), n);

        it=0;
        indices[it++] = 0;    indices[it++] = 1;    indices[it++] = 2;    indices[it++] = 0;    indices[it++] = 2;    indices[it++] = 3;
        indices[it++] = 4;    indices[it++] = 5;    indices[it++] = 6;    indices[it++] = 4;    indices[it++] = 6;    indices[it++] = 7;
        indices[it++] = 8;    indices[it++] = 11;   indices[it++] = 10;   indices[it++] = 8;    indices[it++] = 10;   indices[it++] = 9;
        indices[it++] = 12;   indices[it++] = 15;   indices[it++] = 14;   indices[it++] = 12;   indices[it++] = 14;   indices[it++] = 13;
        indices[it++] = 16;   indices[it++] = 17;   indices[it++] = 18;   indices[it++] = 16;   indices[it++] = 18;   indices[it++] = 19;
        indices[it++] = 20;   indices[it++] = 23;   indices[it++] = 22;   indices[it++] = 20;   indices[it++] = 22;   indices[it++] = 21;

        geometry->setIndices(indices.data(), indices.size());
    }
    return geometry;
}

Geometry* newCircleGeometry(float radius, int n_segments, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    {
        geometry->setPrimitiveMode(GL_TRIANGLES);
        std::vector<glm::vec3> verts;
        std::vector<glm::vec3> colors;
        std::vector<uint> indices;

        verts.push_back({0,0,0});
        colors.push_back(color);
        float a=0.0f;

        for(int i=0; i<n_segments; i++){
            float x = radius * cos(a);
            float y = radius * sin(a);
            verts.push_back({x,y,0});
            colors.push_back(color);
            a += 2*M_PI/n_segments;
        }

        for(int i=0; i<n_segments; i++){
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i+1);
        }
        indices.push_back(0);
        indices.push_back(n_segments);
        indices.push_back(1);


        geometry->setIndices(indices.data(), indices.size());
        geometry->setVertices((int)Attributes::position, verts.data(), verts.size());
        geometry->setAttribute((int)Attributes::color, colors.data(), colors.size());
    }
    return geometry;
}


Geometry* newCylinderGeometry(float radius, int segments, float height, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLES);

    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<uint> indices;

    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));
        float u = (float)i / segments;

        pos.push_back(glm::vec3(x, height / 2.0f, z));
        colors.push_back(color);
        normals.push_back(sideNormal);
        uvs.push_back(glm::vec2(u, 1.0f));


        pos.push_back(glm::vec3(x, -height / 2.0f, z));
        colors.push_back(color);
        normals.push_back(sideNormal);
        uvs.push_back(glm::vec2(u, 0.0f));
    }


    for (int i = 0; i < segments; ++i) {
        uint topLeft     = i * 2;
        uint bottomLeft  = i * 2 + 1;
        uint topRight    = (i + 1) * 2;
        uint bottomRight = (i + 1) * 2 + 1;

        indices.push_back(topLeft);
        indices.push_back(bottomLeft);
        indices.push_back(topRight);

        indices.push_back(topRight);
        indices.push_back(bottomLeft);
        indices.push_back(bottomRight);
    }

    geometry->setIndices(indices.data(), indices.size());
    geometry->setVertices((int)Attributes::position, pos.data(), pos.size());
    geometry->setAttribute((int)Attributes::color, colors.data(), colors.size());
    geometry->setAttribute((int)Attributes::normal, normals.data(), normals.size());
    geometry->setAttribute((int)Attributes::uv1, uvs.data(), uvs.size());

    return geometry;
}
Geometry* newConeGeometry(float radius, float height, int segments, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLES);

    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<uint> indices;

    uint baseCenterIdx = pos.size();
    pos.push_back(glm::vec3(0.0f, -height / 2.0f, 0.0f));
    colors.push_back(color);
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    uvs.push_back(glm::vec2(0.5f, 0.5f));

    uint baseRingStart = pos.size();
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        pos.push_back(glm::vec3(radius * cos(angle), -height / 2.0f, radius * sin(angle)));
        colors.push_back(color);
        normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

        float u_cap = 0.5f + 0.5f * cos(angle);
        float v_cap = 0.5f + 0.5f * sin(angle);
        uvs.push_back(glm::vec2(u_cap, v_cap));
    }
    for (int i = 0; i < segments; ++i) {
        indices.push_back(baseCenterIdx);
        indices.push_back(baseRingStart + i + 1);
        indices.push_back(baseRingStart + i);
    }

    uint sideTipStart = pos.size();
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        float u = (float)i / segments;

        pos.push_back(glm::vec3(0.0f, height / 2.0f, 0.0f));
        colors.push_back(color);
        glm::vec3 n = glm::normalize(glm::vec3(cos(angle), radius / height, sin(angle)));
        normals.push_back(n);
        uvs.push_back(glm::vec2(u, 1.0f));
    }

    uint sideRingStart = pos.size();
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        float u = (float)i / segments;


        pos.push_back(glm::vec3(radius * cos(angle), -height / 2.0f, radius * sin(angle)));
        colors.push_back(color);
        glm::vec3 n = glm::normalize(glm::vec3(cos(angle), radius / height, sin(angle)));
        normals.push_back(n);
        uvs.push_back(glm::vec2(u, 0.0f));
    }


    for (int i = 0; i < segments; ++i) {
        indices.push_back(sideTipStart + i);
        indices.push_back(sideRingStart + i);
        indices.push_back(sideRingStart + i + 1);
    }

    geometry->setIndices(indices.data(), indices.size());
    geometry->setVertices((int)Attributes::position, pos.data(), pos.size());
    geometry->setAttribute((int)Attributes::color, colors.data(), colors.size());
    geometry->setAttribute((int)Attributes::normal, normals.data(), normals.size());

    geometry->setAttribute((int)Attributes::uv1, uvs.data(), uvs.size());

    return geometry;
}


Geometry* newSphereGeometry(float radius, int segments, int rings, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLES);

    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<uint> indices;

    for (int r = 0; r <= rings; ++r) {
        float phi = r * M_PI / rings;
        for (int s = 0; s <= segments; ++s) {
            float theta = s * 2.0f * M_PI / segments;

            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            pos.push_back(glm::vec3(x, y, z));
            colors.push_back(color);
            normals.push_back(glm::normalize(glm::vec3(x, y, z)));


            float u = (float)s / segments;
            float v = 1.0f - ((float)r / rings);
            uvs.push_back(glm::vec2(u, v));
        }
    }

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < segments; ++s) {
            uint i0 = r * (segments + 1) + s;
            uint i1 = i0 + 1;
            uint i2 = i0 + (segments + 1);
            uint i3 = i2 + 1;

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    geometry->setIndices(indices.data(), indices.size());
    geometry->setVertices((int)Attributes::position, pos.data(), pos.size());
    geometry->setAttribute((int)Attributes::color, colors.data(), colors.size());
    geometry->setAttribute((int)Attributes::normal, normals.data(), normals.size());

    geometry->setAttribute((int)Attributes::uv1, uvs.data(), uvs.size());

    return geometry;
}
