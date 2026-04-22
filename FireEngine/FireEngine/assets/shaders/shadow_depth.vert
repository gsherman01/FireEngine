#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 4) in ivec4 aBoneIds;
layout (location = 5) in vec4 aBoneWeights;

uniform mat4 uModel;
uniform mat4 uLightSpaceMatrix;
uniform bool uHasSkinning;
uniform mat4 uBoneMatrices[100];

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
    gl_Position = uLightSpaceMatrix * uModel * localPosition;
}
