#version 330 core

in vec3 vWorldPosition;
in vec3 vNormal;
in vec4 vLightSpacePosition;

uniform vec3 uObjectColor;
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightColor;
uniform vec3 uDirectionalLightDirection;
uniform vec3 uDirectionalLightColor;
uniform vec3 uCameraPosition;
uniform float uShininess;
uniform sampler2D uShadowMap;

out vec4 FragColor;

vec3 CalculateBlinnPhong(vec3 normal, vec3 lightDirection, vec3 lightColor, vec3 viewDirection)
{
    float diffuseStrength = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), uShininess);
    vec3 specular = 0.35 * specularStrength * lightColor;

    return diffuse + specular;
}

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

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDirection = normalize(uCameraPosition - vWorldPosition);

    vec3 pointDirection = normalize(uPointLightPosition - vWorldPosition);
    vec3 directionalDirection = normalize(-uDirectionalLightDirection);

    float environmentFactor = clamp(normal.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 environment = mix(vec3(0.05, 0.08, 0.12), vec3(0.18, 0.20, 0.24), environmentFactor);
    vec3 lighting = environment;
    lighting += CalculateBlinnPhong(normal, pointDirection, uPointLightColor, viewDirection);
    float shadow = CalculateShadow(vLightSpacePosition, normal, directionalDirection);
    lighting += (1.0 - shadow) * CalculateBlinnPhong(normal, directionalDirection, uDirectionalLightColor, viewDirection);

    FragColor = vec4(lighting * uObjectColor, 1.0);
}
