#include "AnimationSystem.h"

#include "ECS/Registry.h"
#include "Scene/Components.h"

void AnimationSystem::Update(Registry& registry, float deltaTime) const
{
    for (EntityId entityId : registry.GetSystemEntities<AnimationSystem>())
    {
        Entity entity(entityId, &registry);
        AnimationComponent& animation = entity.GetComponent<AnimationComponent>();
        TransformComponent& transform = entity.GetComponent<TransformComponent>();

        animation.playbackTime += deltaTime;
        transform.transform.rotationDegrees.y += 20.0f * deltaTime;
    }
}
