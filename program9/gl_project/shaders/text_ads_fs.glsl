#version 330 core

uniform sampler2D TextureSampler;
uniform bool UseTexture;
uniform bool FlipTree;

uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;

uniform vec3 FogColor;
uniform float FogDensity;

in vec3 v_normalEye;
in vec3 v_positionEye;
in vec2 v_uv;

out vec4 FragColor;

void main()
{
    vec2 correctedUV = v_uv;
    if(FlipTree) {
        correctedUV.y = 1.0 - v_uv.y;
    }

    vec4 texColor = vec4(1.0);
    float customAlpha = 1.0;

    if(UseTexture) {
        texColor = texture(TextureSampler, correctedUV);

        if(!FlipTree) {
            if(texColor.r > 0.2 || texColor.g > 0.2 || texColor.b > 0.2) {
                customAlpha = 0.18;
            } else {
                customAlpha = 1.0;
            }
        }

        if(FlipTree) {
            if(texColor.a < 0.1 || (texColor.r < 0.05 && texColor.g < 0.05 && texColor.b < 0.05)) {
                discard;
            }
            customAlpha = 1.0;
        }
    }

    vec3 baseColor = MaterialDiffuse;
    if(UseTexture) {
        baseColor = texColor.rgb;
    }

    vec3 ambient = LightColor * MaterialAmbient * baseColor;
    vec3 n = normalize(v_normalEye);
    vec3 l = normalize(LightPosition - v_positionEye);
    float diffFactor = max(0.0, dot(n, l));
    vec3 diffuse = diffFactor * LightColor * baseColor;

    vec3 finalColor = ambient + diffuse;

    float dist = length(v_positionEye);


    float fogFactor = exp(-pow(FogDensity * dist, 2.0));

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    finalColor = mix(FogColor, finalColor, fogFactor);

    FragColor = vec4(finalColor, customAlpha);
}
