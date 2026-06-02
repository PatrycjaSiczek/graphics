#version 420

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexUV;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMat;
uniform bool IsSphere;

out vec3 v_positionEye;
out vec3 v_normalEye;
out vec2 v_uv;

void main()
{
    vec4 posEye = ModelViewMatrix * vec4(VertexPosition, 1.0);
    v_positionEye = posEye.xyz;
    v_normalEye = normalize(NormalMatrix * VertexNormal);

    if(IsSphere) {
        v_uv = vec2(VertexUV.y, 1.0 - VertexUV.x);
    } else {
        v_uv = VertexUV;
    }

    gl_Position = ProjectionMat * posEye;
}
