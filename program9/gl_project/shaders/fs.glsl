#version 420

uniform vec3 LightPosition;
uniform vec3 LightColor;

uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;
uniform float MaterialShiness;

uniform sampler2D TextureSampler;
uniform bool UseTexture;

uniform vec3 FogColor;
uniform float FogMinDist;
uniform float FogMaxDist;

in vec3 v_positionEye;
in vec3 v_normalEye;
in vec2 v_uv;

out vec4 FragColor;

void main()
{
    vec4 texColor = vec4(1.0);
    if(UseTexture) {
        texColor = texture(TextureSampler, v_uv);
    }

    vec3 baseColor = MaterialDiffuse;
    if (UseTexture) {
        baseColor = texColor.rgb;
    }

    vec3 ambient = LightColor * MaterialAmbient * baseColor;
    vec3 n = normalize(v_normalEye);
    vec3 l = normalize(LightPosition - v_positionEye);
    float diffFactor = max(0.0, dot(n, l));
    vec3 diffuse = diffFactor * LightColor * baseColor;

    vec3 finalColor = ambient + diffuse;

    float depth = gl_FragCoord.z;

}
