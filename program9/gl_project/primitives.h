#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "geometry.h"

Geometry *newAxesGeometry();

Geometry *newPlaneGeometry(glm::vec2 size, glm::vec3 color);

Geometry* newBoxGeometry(glm::vec3 size, glm::vec3 color);

Geometry* newCircleGeometry(float radius, int n_segments, glm::vec3 color);

Geometry* newCylinderGeometry(float radius, int segments, float height, glm::vec3 color);

Geometry* newConeGeometry(float radius, float height, int segments, glm::vec3 color);
Geometry* newSphereGeometry(float radius, int u_segments, int v_segments, glm::vec3 color);


#endif // PRIMITIVES_H
