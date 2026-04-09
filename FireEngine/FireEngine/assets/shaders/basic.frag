#version 330 core

in vec3 vWorldPosition;
in vec3 vNormal;

uniform vec3 uObjectColor;
uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform vec3 uCameraPosition;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDirection = normalize(uLightPosition - vWorldPosition);
    vec3 viewDirection = normalize(uCameraPosition - vWorldPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * uLightColor;

    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diffuseFactor * uLightColor;

    float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = 0.35 * specularFactor * uLightColor;

    vec3 finalColor = (ambient + diffuse + specular) * uObjectColor;
    FragColor = vec4(finalColor, 1.0);
}
