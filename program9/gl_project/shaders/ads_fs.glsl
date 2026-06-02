#version 420

in vec3 v_normalEye;
in vec3 v_positionEye;
in vec2 v_uv;

uniform vec3 LightPosition;
uniform vec3 LightColor;

uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;
uniform float MaterialShiness;

uniform sampler2D TextureSampler;
uniform bool UseTexture;

out vec4 FragColor;

void main()
{

    vec3 ambient = LightColor * MaterialAmbient;


    vec3 n = normalize(v_normalEye);
    vec3 l = normalize(LightPosition - v_positionEye);
    float diffFactor = max(0.0, dot(n, l));
    vec3 diffuse = diffFactor * LightColor * MaterialDiffuse;


    vec3 r = reflect(-l, n);
    vec3 v = normalize(-v_positionEye);
    float specFactor = pow(max(0.0, dot(r, v)), MaterialShiness);
    vec3 specular = specFactor * LightColor * MaterialSpecular;

    vec3 adsColor = ambient + diffuse + specular;

    if (UseTexture) {
        vec4 texColor = texture(TextureSampler, v_uv);
        FragColor = vec4(adsColor, 1.0) * texColor;
    } else {
        FragColor = vec4(adsColor, 1.0);
    }
}
