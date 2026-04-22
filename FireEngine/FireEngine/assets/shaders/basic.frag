#version 330 core

struct Light
{
    int type; // 0 directional, 1 point
    vec3 color;
    vec3 direction;
    vec3 position;
    float intensity;
    float range;
};

in vec3 vWorldPosition;
in vec3 vNormal;
in vec2 vUV;
in vec4 vLightSpacePosition;

uniform Light uLights[8];
uniform int uLightCount;
uniform vec3 uAlbedo;
uniform vec3 uCameraPosition;
uniform float uShininess;
uniform sampler2D uShadowMap;
uniform sampler2D uDiffuseTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uNormalTexture;
uniform bool uHasDiffuseTexture;
uniform bool uHasSpecularTexture;
uniform bool uHasNormalTexture;

out vec4 FragColor;

float CalculateShadow(vec4 lightSpacePosition, vec3 normal, vec3 lightDirection)
{
    vec3 projectionCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projectionCoords = projectionCoords * 0.5 + 0.5;
    if (projectionCoords.z > 1.0)
    {
        return 0.0;
    }

    float closestDepth = texture(uShadowMap, projectionCoords.xy).r;
    float currentDepth = projectionCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDirection)), 0.0005);
    return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
}

vec3 BlinnPhong(vec3 N, vec3 L, vec3 V, vec3 lightColor, float intensity, float attenuation)
{
    float diff = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float specPow = uHasSpecularTexture ? texture(uSpecularTexture, vUV).r * 128.0 : uShininess;
    float spec = pow(max(dot(N, H), 0.0), max(specPow, 1.0));
    return (diff + spec * 0.25) * lightColor * intensity * attenuation;
}

void main()
{
    vec3 baseColor = uHasDiffuseTexture ? texture(uDiffuseTexture, vUV).rgb : uAlbedo;
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(uCameraPosition - vWorldPosition);

    vec3 lighting = mix(vec3(0.05, 0.07, 0.12), vec3(0.2, 0.23, 0.27), clamp(normal.y * 0.5 + 0.5, 0.0, 1.0));

    for (int i = 0; i < uLightCount; ++i)
    {
        vec3 lightDir;
        float attenuation = 1.0;

        if (uLights[i].type == 0)
        {
            lightDir = normalize(-uLights[i].direction);
            float shadow = CalculateShadow(vLightSpacePosition, normal, lightDir);
            lighting += (1.0 - shadow) * BlinnPhong(normal, lightDir, viewDir, uLights[i].color, uLights[i].intensity, attenuation);
        }
        else
        {
            vec3 delta = uLights[i].position - vWorldPosition;
            float distanceToLight = length(delta);
            lightDir = normalize(delta);
            attenuation = 1.0 / (1.0 + (distanceToLight * distanceToLight) / max(uLights[i].range, 0.001));
            lighting += BlinnPhong(normal, lightDir, viewDir, uLights[i].color, uLights[i].intensity, attenuation);
        }
    }

    FragColor = vec4(baseColor * lighting, 1.0);
}
