#include "Renderer.h"

#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h>

#include <array>

namespace
{
    unsigned int GetSkyboxVao()
    {
        static const float vertices[] = {
            -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

        static unsigned int vao = 0;
        static unsigned int vbo = 0;

        if (vao == 0)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        }

        return vao;
    }
}

void Renderer::BeginFrame(const glm::vec3& clearColor) const
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// DrawMesh binds per-object/per-frame GPU state (matrices, lights, material params, textures)
// then submits indexed geometry for rasterization.
void Renderer::DrawMesh(
    const Mesh& mesh,
    const Shader& shader,
    const Material& material,
    const glm::mat4& model,
    const Camera& camera,
    float aspectRatio,
    const std::vector<RenderLight>& lights,
    const glm::mat4& lightSpaceMatrix,
    unsigned int shadowMapTexture,
    const std::vector<glm::mat4>& boneMatrices) const
{
    shader.Use();

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", camera.GetViewMatrix());
    shader.SetMat4("uProjection", camera.GetProjectionMatrix(aspectRatio));
    shader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);
    shader.SetVec3("uCameraPosition", camera.GetPosition());
    shader.SetVec3("uAlbedo", material.albedo);
    shader.SetFloat("uShininess", material.shininess);
    shader.SetFloat("uMetallic", material.metallic);
    shader.SetFloat("uRoughness", material.roughness);
    shader.SetFloat("uAO", material.ao);
    shader.SetVec3("uEmissive", material.emissive);

    shader.SetInt("uLightCount", static_cast<int>(lights.size()));
    for (int i = 0; i < static_cast<int>(lights.size()); ++i)
    {
        const std::string index = "uLights[" + std::to_string(i) + "]";
        shader.SetInt(index + ".type", lights[i].type);
        shader.SetVec3(index + ".color", lights[i].color);
        shader.SetVec3(index + ".direction", lights[i].direction);
        shader.SetVec3(index + ".position", lights[i].position);
        shader.SetFloat(index + ".intensity", lights[i].intensity);
        shader.SetFloat(index + ".range", lights[i].range);
    }

    unsigned int texUnit = 0;
    if (material.diffuseTexture != nullptr)
    {
        shader.SetInt("uDiffuseTexture", static_cast<int>(texUnit));
        shader.SetInt("uHasDiffuseTexture", 1);
        material.diffuseTexture->Bind(texUnit++);
    }
    else
    {
        shader.SetInt("uHasDiffuseTexture", 0);
    }

    if (material.specularTexture != nullptr)
    {
        shader.SetInt("uSpecularTexture", static_cast<int>(texUnit));
        shader.SetInt("uHasSpecularTexture", 1);
        material.specularTexture->Bind(texUnit++);
    }
    else
    {
        shader.SetInt("uHasSpecularTexture", 0);
    }

    if (material.normalTexture != nullptr)
    {
        shader.SetInt("uNormalTexture", static_cast<int>(texUnit));
        shader.SetInt("uHasNormalTexture", 1);
        material.normalTexture->Bind(texUnit++);
    }
    else
    {
        shader.SetInt("uHasNormalTexture", 0);
    }

    shader.SetInt("uShadowMap", static_cast<int>(texUnit));
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

    shader.SetInt("uHasSkinning", boneMatrices.empty() ? 0 : 1);
    constexpr int kMaxBones = 100;
    for (int i = 0; i < static_cast<int>(boneMatrices.size()) && i < kMaxBones; ++i)
    {
        shader.SetMat4("uBoneMatrices[" + std::to_string(i) + "]", boneMatrices[i]);
    }

    mesh.Draw();
}

void Renderer::DrawMeshDepth(
    const Mesh& mesh,
    const Shader& depthShader,
    const glm::mat4& model,
    const glm::mat4& lightSpaceMatrix,
    const std::vector<glm::mat4>& boneMatrices) const
{
    depthShader.Use();
    depthShader.SetMat4("uModel", model);
    depthShader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);
    depthShader.SetInt("uHasSkinning", boneMatrices.empty() ? 0 : 1);

    constexpr int kMaxBones = 100;
    for (int i = 0; i < static_cast<int>(boneMatrices.size()) && i < kMaxBones; ++i)
    {
        depthShader.SetMat4("uBoneMatrices[" + std::to_string(i) + "]", boneMatrices[i]);
    }

    mesh.Draw();
}

void Renderer::DrawSkybox(const Shader& shader, const Camera& camera, float aspectRatio, unsigned int cubemapTexture) const
{
    shader.Use();

    glm::mat4 view = camera.GetViewMatrix();
    view[3] = glm::vec4(0, 0, 0, 1);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", camera.GetProjectionMatrix(aspectRatio));
    shader.SetInt("uSkybox", 0);

    glDepthFunc(GL_LEQUAL);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glBindVertexArray(GetSkyboxVao());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
