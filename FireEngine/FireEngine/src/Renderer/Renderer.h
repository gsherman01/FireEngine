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
        const glm::vec3& lightPosition,
        const glm::vec3& lightColor) const;
};
