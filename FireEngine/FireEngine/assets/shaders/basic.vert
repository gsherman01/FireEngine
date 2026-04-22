#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIds;
layout (location = 5) in vec4 aBoneWeights;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;
uniform bool uHasSkinning;
uniform mat4 uBoneMatrices[100];

out vec3 vWorldPosition;
out vec3 vNormal;
out vec2 vUV;
out vec4 vLightSpacePosition;

void main()
{
    mat4 skinMatrix = mat4(1.0);
    if (uHasSkinning)
    {
        skinMatrix = mat4(0.0);
        for (int i = 0; i < 4; ++i)
        {
            if (aBoneIds[i] >= 0)
            {
                skinMatrix += uBoneMatrices[aBoneIds[i]] * aBoneWeights[i];
            }
        }
    }

    vec4 localPosition = uHasSkinning ? (skinMatrix * vec4(aPosition, 1.0)) : vec4(aPosition, 1.0);
    vec3 localNormal = uHasSkinning ? mat3(skinMatrix) * aNormal : aNormal;

    vec4 worldPosition = uModel * localPosition;
    vWorldPosition = worldPosition.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * localNormal;
    vUV = aUV;
    vLightSpacePosition = uLightSpaceMatrix * worldPosition;

    gl_Position = uProjection * uView * worldPosition;
}
