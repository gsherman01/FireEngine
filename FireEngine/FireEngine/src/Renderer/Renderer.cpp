#include "Renderer.h"

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h>

void Renderer::BeginFrame(const glm::vec3& clearColor) const
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawMesh(
    const Mesh& mesh,
    const Shader& shader,
    const glm::mat4& model,
    const Camera& camera,
    float aspectRatio,
    const glm::vec3& objectColor,
    const glm::vec3& lightPosition,
    const glm::vec3& lightColor) const
{
    shader.Use();

    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);

    shader.SetVec3("uObjectColor", objectColor);
    shader.SetVec3("uLightPosition", lightPosition);
    shader.SetVec3("uLightColor", lightColor);
    shader.SetVec3("uCameraPosition", camera.GetPosition());

    mesh.Draw();
}
