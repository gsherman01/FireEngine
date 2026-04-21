#include "CameraSystem.h"

#include "Core/Input.h"
#include "ECS/Registry.h"
#include "Scene/Components.h"

#include <GLFW/glfw3.h>

void CameraSystem::Update(Registry& registry, float deltaTime) const
{
    for (EntityId entityId : registry.GetSystemEntities<CameraSystem>())
    {
        Entity entity(entityId, &registry);
        CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
        if (!cameraComponent.primary)
        {
            continue;
        }

        cameraComponent.camera.ProcessKeyboard(
            Input::IsKeyPressed(GLFW_KEY_W),
            Input::IsKeyPressed(GLFW_KEY_S),
            Input::IsKeyPressed(GLFW_KEY_A),
            Input::IsKeyPressed(GLFW_KEY_D),
            deltaTime);

        cameraComponent.camera.ProcessMouse(Input::GetMouseDeltaX(), Input::GetMouseDeltaY());
    }
}
