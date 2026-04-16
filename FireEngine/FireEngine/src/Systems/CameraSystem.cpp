#include "CameraSystem.h"

#include "Core/Input.h"
#include "ECS/Registry.h"
#include "Scene/Components.h"

#include <GLFW/glfw3.h>

void CameraSystem::Update(Registry& registry, float deltaTime) const
{
    for (Entity entity : registry.View<CameraComponent>())
    {
        CameraComponent& cameraComponent = registry.GetComponent<CameraComponent>(entity);
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
