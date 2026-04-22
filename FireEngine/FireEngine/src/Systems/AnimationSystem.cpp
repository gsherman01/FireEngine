#include "AnimationSystem.h"

#include "ECS/Registry.h"
#include "Scene/Components.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

void AnimationSystem::Update(Registry& registry, float deltaTime) const
{
    for (EntityId entityId : registry.GetSystemEntities<AnimationSystem>())
    {
        Entity entity(entityId, &registry);
        AnimationComponent& animation = entity.GetComponent<AnimationComponent>();
        animation.playbackTime += deltaTime * animation.speed;

        if (!entity.HasComponent<MeshComponent>())
        {
            continue;
        }

        const MeshComponent& mesh = entity.GetComponent<MeshComponent>();
        if (mesh.model == nullptr || !mesh.model->HasBones())
        {
            continue;
        }

        animation.boneMatrices.resize(mesh.model->bones.size(), glm::mat4(1.0f));
        for (std::size_t boneIndex = 0; boneIndex < mesh.model->bones.size(); ++boneIndex)
        {
            const float wiggle = std::sin(animation.playbackTime + static_cast<float>(boneIndex) * 0.35f) * 0.2f;
            glm::mat4 transform = glm::rotate(glm::mat4(1.0f), wiggle, glm::vec3(0.0f, 1.0f, 0.0f));
            animation.boneMatrices[boneIndex] = transform * mesh.model->bones[boneIndex].offset;
        }
    }
}
