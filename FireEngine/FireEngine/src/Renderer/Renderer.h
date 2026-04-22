#pragma once

#include <glm/glm.hpp>

#include <vector>

class Camera;
class Material;
class Mesh;
class Shader;

struct RenderLight
{
    int type = 0;
    glm::vec3 color = glm::vec3(1.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    float intensity = 1.0f;
    float range = 15.0f;
};

class Renderer
{
public:
    void BeginFrame(const glm::vec3& clearColor) const;

    void DrawMesh(
        const Mesh& mesh,
        const Shader& shader,
        const Material& material,
        const glm::mat4& model,
        const Camera& camera,
        float aspectRatio,
        const std::vector<RenderLight>& lights,
        const glm::mat4& lightSpaceMatrix,
        unsigned int shadowMapTexture,
        const std::vector<glm::mat4>& boneMatrices) const;

    void DrawMeshDepth(
        const Mesh& mesh,
        const Shader& depthShader,
        const glm::mat4& model,
        const glm::mat4& lightSpaceMatrix,
        const std::vector<glm::mat4>& boneMatrices) const;

    void DrawSkybox(const Shader& shader, const Camera& camera, float aspectRatio, unsigned int cubemapTexture) const;
};
