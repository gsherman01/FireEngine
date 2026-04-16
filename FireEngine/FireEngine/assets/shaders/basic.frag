#version 330 core

in vec3 vWorldPosition;
in vec3 vNormal;

uniform vec3 uObjectColor;
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightColor;
uniform vec3 uDirectionalLightDirection;
uniform vec3 uDirectionalLightColor;
uniform vec3 uCameraPosition;
uniform float uShininess;

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

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDirection = normalize(uCameraPosition - vWorldPosition);

    vec3 pointDirection = normalize(uPointLightPosition - vWorldPosition);
    vec3 directionalDirection = normalize(-uDirectionalLightDirection);

    vec3 lighting = vec3(0.1) * (uPointLightColor + uDirectionalLightColor);
    lighting += CalculateBlinnPhong(normal, pointDirection, uPointLightColor, viewDirection);
    lighting += CalculateBlinnPhong(normal, directionalDirection, uDirectionalLightColor, viewDirection);

    FragColor = vec4(lighting * uObjectColor, 1.0);
}
