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
    const glm::vec3& pointLightPosition,
    const glm::vec3& pointLightColor,
    const glm::vec3& directionalLightDirection,
    const glm::vec3& directionalLightColor,
    const glm::mat4& lightSpaceMatrix,
    unsigned int shadowMapTexture,
    float shininess) const
{
    shader.Use();

    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);

    shader.SetVec3("uObjectColor", objectColor);
    shader.SetVec3("uPointLightPosition", pointLightPosition);
    shader.SetVec3("uPointLightColor", pointLightColor);
    shader.SetVec3("uDirectionalLightDirection", directionalLightDirection);
    shader.SetVec3("uDirectionalLightColor", directionalLightColor);
    shader.SetVec3("uCameraPosition", camera.GetPosition());
    shader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);
    shader.SetInt("uShadowMap", 0);
    shader.SetFloat("uShininess", shininess);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

    mesh.Draw();
}

void Renderer::DrawMeshDepth(
    const Mesh& mesh,
    const Shader& depthShader,
    const glm::mat4& model,
    const glm::mat4& lightSpaceMatrix) const
{
    depthShader.Use();
    depthShader.SetMat4("uModel", model);
    depthShader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);
    mesh.Draw();
}
