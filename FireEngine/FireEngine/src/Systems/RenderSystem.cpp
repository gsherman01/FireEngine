#include "RenderSystem.h"

#include "ECS/Registry.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Scene/Components.h"

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
        if (mesh.mesh == nullptr)
        {
            continue;
        }

        renderer.DrawMeshDepth(*mesh.mesh, depthShader, transform.transform.ToMatrix(), lightSpaceMatrix);
    }
}

void RenderSystem::RenderScene(
    Registry& registry,
    const Renderer& renderer,
    const Shader& shader,
    int viewportWidth,
    int viewportHeight,
    const glm::mat4& lightSpaceMatrix,
    unsigned int shadowMapTexture) const
{
    if (viewportWidth <= 0 || viewportHeight <= 0)
    {
        return;
    }

    const float aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
    const glm::vec3 directionalColor = ResolveDirectionalLightColor(registry);
    const glm::vec3 directionalDirection = ResolveDirectionalLightDirection(registry);
    const glm::vec3 pointPosition = ResolvePointLightPosition(registry);
    const glm::vec3 pointColor = ResolvePointLightColor(registry);

    Entity activeCamera;
    for (Entity entity : registry.View<CameraComponent>())
    {
        const CameraComponent& camera = entity.GetComponent<CameraComponent>();
        if (camera.primary)
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

    for (EntityId entityId : registry.GetSystemEntities<RenderSystem>())
    {
        Entity entity(entityId, &registry);
        const TransformComponent& transform = entity.GetComponent<TransformComponent>();
        const MeshComponent& mesh = entity.GetComponent<MeshComponent>();
        const MaterialComponent& material = entity.GetComponent<MaterialComponent>();

        if (mesh.mesh == nullptr)
        {
            continue;
        }

        renderer.DrawMesh(
            *mesh.mesh,
            shader,
            transform.transform.ToMatrix(),
            camera,
            aspectRatio,
            material.albedo,
            pointPosition,
            pointColor,
            directionalDirection,
            directionalColor,
            lightSpaceMatrix,
            shadowMapTexture,
            material.shininess);
    }
}

glm::vec3 RenderSystem::ResolveDirectionalLightColor(Registry& registry) const
{
    for (Entity entity : registry.View<LightComponent>())
    {
        const LightComponent& light = entity.GetComponent<LightComponent>();
        if (light.type == LightType::Directional)
        {
            return light.color * light.intensity;
        }
    }

    return glm::vec3(0.0f);
}

glm::vec3 RenderSystem::ResolveDirectionalLightDirection(Registry& registry) const
{
    for (Entity entity : registry.View<LightComponent>())
    {
        const LightComponent& light = entity.GetComponent<LightComponent>();
        if (light.type == LightType::Directional)
        {
            return light.direction;
        }
    }

    return glm::vec3(-0.2f, -1.0f, -0.3f);
}

glm::vec3 RenderSystem::ResolvePointLightPosition(Registry& registry) const
{
    for (Entity entity : registry.View<LightComponent>())
    {
        const LightComponent& light = entity.GetComponent<LightComponent>();
        if (light.type == LightType::Point)
        {
            return light.position;
        }
    }

    return glm::vec3(2.0f, 2.0f, 2.0f);
}

glm::vec3 RenderSystem::ResolvePointLightColor(Registry& registry) const
{
    for (Entity entity : registry.View<LightComponent>())
    {
        const LightComponent& light = entity.GetComponent<LightComponent>();
        if (light.type == LightType::Point)
        {
            return light.color * light.intensity;
        }
    }

    return glm::vec3(1.0f);
}
