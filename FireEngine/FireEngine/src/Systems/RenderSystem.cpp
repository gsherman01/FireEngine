#include "RenderSystem.h"

#include "ECS/Registry.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Scene/Components.h"

void RenderSystem::RenderScene(
    Registry& registry,
    const Renderer& renderer,
    const Shader& shader,
    int viewportWidth,
    int viewportHeight) const
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

    Entity activeCamera = InvalidEntity;
    for (Entity entity : registry.View<CameraComponent>())
    {
        const CameraComponent& camera = registry.GetComponent<CameraComponent>(entity);
        if (camera.primary)
        {
            activeCamera = entity;
            break;
        }
    }

    if (activeCamera == InvalidEntity)
    {
        return;
    }

    const Camera& camera = registry.GetComponent<CameraComponent>(activeCamera).camera;

    for (Entity entity : registry.View<TransformComponent, MeshComponent, MaterialComponent>())
    {
        const TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
        const MeshComponent& mesh = registry.GetComponent<MeshComponent>(entity);
        const MaterialComponent& material = registry.GetComponent<MaterialComponent>(entity);

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
            material.shininess);
    }
}

glm::vec3 RenderSystem::ResolveDirectionalLightColor(Registry& registry) const
{
    for (Entity entity : registry.View<LightComponent>())
    {
        const LightComponent& light = registry.GetComponent<LightComponent>(entity);
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
        const LightComponent& light = registry.GetComponent<LightComponent>(entity);
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
        const LightComponent& light = registry.GetComponent<LightComponent>(entity);
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
        const LightComponent& light = registry.GetComponent<LightComponent>(entity);
        if (light.type == LightType::Point)
        {
            return light.color * light.intensity;
        }
    }

    return glm::vec3(1.0f);
}
