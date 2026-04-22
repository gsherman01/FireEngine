#include "RenderSystem.h"

#include "ECS/Registry.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Scene/Components.h"

std::vector<RenderLight> RenderSystem::CollectLights(Registry& registry) const
{
    std::vector<RenderLight> lights;
    lights.reserve(8);

    for (Entity entity : registry.View<LightComponent>())
    {
        if (lights.size() >= 8)
        {
            break;
        }

        const LightComponent& light = entity.GetComponent<LightComponent>();
        RenderLight renderLight;
        renderLight.type = light.type == LightType::Directional ? 0 : 1;
        renderLight.color = light.color;
        renderLight.direction = light.direction;
        renderLight.position = light.position;
        renderLight.intensity = light.intensity;
        renderLight.range = light.range;
        lights.push_back(renderLight);
    }

    return lights;
}

void RenderSystem::RenderShadowMap(
    Registry& registry,
    const Renderer& renderer,
    const Shader& depthShader,
    const glm::mat4& lightSpaceMatrix) const
{
    for (EntityId entityId : registry.GetSystemEntities<RenderSystem>())
    {
        Entity entity(entityId, &registry);
        const TransformComponent& transform = entity.GetComponent<TransformComponent>();
        const MeshComponent& mesh = entity.GetComponent<MeshComponent>();

        if (mesh.model == nullptr)
        {
            continue;
        }

        const std::vector<glm::mat4> boneMatrices = entity.HasComponent<AnimationComponent>()
            ? entity.GetComponent<AnimationComponent>().boneMatrices
            : std::vector<glm::mat4>{};

        for (const ModelMesh& modelMesh : mesh.model->meshes)
        {
            if (modelMesh.mesh == nullptr)
            {
                continue;
            }

            renderer.DrawMeshDepth(*modelMesh.mesh, depthShader, transform.transform.ToMatrix(), lightSpaceMatrix, boneMatrices);
        }
    }
}

void RenderSystem::RenderScene(
    Registry& registry,
    const Renderer& renderer,
    const Shader& fallbackShader,
    const Shader& skyboxShader,
    int viewportWidth,
    int viewportHeight,
    const glm::mat4& lightSpaceMatrix,
    unsigned int shadowMapTexture,
    unsigned int skyboxTexture) const
{
    if (viewportWidth <= 0 || viewportHeight <= 0)
    {
        return;
    }

    Entity activeCamera;
    for (Entity entity : registry.View<CameraComponent>())
    {
        if (entity.GetComponent<CameraComponent>().primary)
        {
            activeCamera = entity;
            break;
        }
    }

    if (!activeCamera.IsValid())
    {
        return;
    }

    const Camera& camera = activeCamera.GetComponent<CameraComponent>().camera;
    const float aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
    const std::vector<RenderLight> lights = CollectLights(registry);

    for (EntityId entityId : registry.GetSystemEntities<RenderSystem>())
    {
        Entity entity(entityId, &registry);
        const TransformComponent& transform = entity.GetComponent<TransformComponent>();
        const MeshComponent& mesh = entity.GetComponent<MeshComponent>();
        const MaterialComponent& materialComponent = entity.GetComponent<MaterialComponent>();

        if (mesh.model == nullptr)
        {
            continue;
        }

        const std::vector<glm::mat4> boneMatrices = entity.HasComponent<AnimationComponent>()
            ? entity.GetComponent<AnimationComponent>().boneMatrices
            : std::vector<glm::mat4>{};

        for (const ModelMesh& modelMesh : mesh.model->meshes)
        {
            if (modelMesh.mesh == nullptr)
            {
                continue;
            }

            Material material = materialComponent.overrideMaterial;
            if (materialComponent.useModelMaterial && modelMesh.materialIndex < mesh.model->materials.size())
            {
                material = mesh.model->materials[modelMesh.materialIndex];
            }

            const Shader& shader = material.shader != nullptr ? *material.shader : fallbackShader;
            renderer.DrawMesh(
                *modelMesh.mesh,
                shader,
                material,
                transform.transform.ToMatrix(),
                camera,
                aspectRatio,
                lights,
                lightSpaceMatrix,
                shadowMapTexture,
                boneMatrices);
        }
    }

    if (skyboxTexture != 0)
    {
        renderer.DrawSkybox(skyboxShader, camera, aspectRatio, skyboxTexture);
    }
}
