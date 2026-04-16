#include "AnimationSystem.h"

#include "ECS/Registry.h"
#include "Scene/Components.h"

void AnimationSystem::Update(Registry& registry, float deltaTime) const
{
    for (Entity entity : registry.View<AnimationComponent, TransformComponent>())
    {
        AnimationComponent& animation = registry.GetComponent<AnimationComponent>(entity);
        TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

        animation.playbackTime += deltaTime;
        transform.transform.rotationDegrees.y += 20.0f * deltaTime;
    }
}
