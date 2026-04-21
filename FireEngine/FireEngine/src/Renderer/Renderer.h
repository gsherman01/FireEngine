#pragma once

#include <glm/glm.hpp>

class Shader;
class Mesh;
class Camera;

class Renderer
{
public:
    void BeginFrame(const glm::vec3& clearColor) const;

    void DrawMesh(
        const Mesh& mesh,
        const Shader& shader,
        const glm::mat4& model,
        const Camera& camera,
        float aspectRatio,
        const glm::vec3& objectColor,
        const glm::vec3& pointLightPosition,
        const glm::vec3& pointLightColor,
        const glm::vec3& directionalLightDirection,
        const glm::vec3& directionalLightColor,
        const glm::mat4& lightSpaceMatrix,
        unsigned int shadowMapTexture,
        float shininess) const;

    void DrawMeshDepth(
        const Mesh& mesh,
        const Shader& depthShader,
        const glm::mat4& model,
        const glm::mat4& lightSpaceMatrix) const;
};
