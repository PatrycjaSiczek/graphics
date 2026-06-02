#version 420

layout (location=0) in vec3 VertexPosition;
layout (location=2) in vec3 VertexNormal;
layout (location=7) in vec2 VertexUv;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMat;

uniform vec2 UVOffset;
uniform bool EnableUVAnimation;

out vec3 v_normalEye;
out vec3 v_positionEye;
out vec2 v_uv;

void main()
{
    vec4 eyePos4 = ModelViewMatrix * vec4(VertexPosition, 1.0);
    v_positionEye = eyePos4.xyz;

    v_normalEye = normalize(NormalMatrix * VertexNormal);

    if (EnableUVAnimation) {
        v_uv = VertexUv + UVOffset;
    } else {
        v_uv = VertexUv;
    }

    gl_Position = ProjectionMat * eyePos4;
}
