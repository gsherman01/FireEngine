#include "AnimationSystem.h"

#include "ECS/Registry.h"
#include "Scene/Components.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <cmath>

namespace
{
    template <typename TKey>
    std::size_t FindKeyIndex(const std::vector<TKey>& keys, double time)
    {
        if (keys.size() < 2)
        {
            return 0;
        }

        for (std::size_t i = 0; i + 1 < keys.size(); ++i)
        {
            if (time < keys[i + 1].time)
            {
                return i;
            }
        }

        return keys.size() - 2;
    }

    glm::vec3 SamplePosition(const BoneAnimationChannel& channel, double time)
    {
        if (channel.positions.empty())
        {
            return glm::vec3(0.0f);
        }

        if (channel.positions.size() == 1)
        {
            return channel.positions.front().value;
        }

        const std::size_t index = FindKeyIndex(channel.positions, time);
        const PositionKey& a = channel.positions[index];
        const PositionKey& b = channel.positions[index + 1];

        const double delta = std::max(0.0001, b.time - a.time);
        const float t = static_cast<float>((time - a.time) / delta);
        return glm::mix(a.value, b.value, glm::clamp(t, 0.0f, 1.0f));
    }

    glm::quat SampleRotation(const BoneAnimationChannel& channel, double time)
    {
        if (channel.rotations.empty())
        {
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }

        if (channel.rotations.size() == 1)
        {
            return glm::normalize(channel.rotations.front().value);
        }

        const std::size_t index = FindKeyIndex(channel.rotations, time);
        const RotationKey& a = channel.rotations[index];
        const RotationKey& b = channel.rotations[index + 1];

        const double delta = std::max(0.0001, b.time - a.time);
        const float t = static_cast<float>((time - a.time) / delta);
        return glm::normalize(glm::slerp(a.value, b.value, glm::clamp(t, 0.0f, 1.0f)));
    }

    glm::vec3 SampleScale(const BoneAnimationChannel& channel, double time)
    {
        if (channel.scales.empty())
        {
            return glm::vec3(1.0f);
        }

        if (channel.scales.size() == 1)
        {
            return channel.scales.front().value;
        }

        const std::size_t index = FindKeyIndex(channel.scales, time);
        const ScaleKey& a = channel.scales[index];
        const ScaleKey& b = channel.scales[index + 1];

        const double delta = std::max(0.0001, b.time - a.time);
        const float t = static_cast<float>((time - a.time) / delta);
        return glm::mix(a.value, b.value, glm::clamp(t, 0.0f, 1.0f));
    }
}

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

        if (!mesh.model->HasAnimations() || mesh.model->animations.front().channels.empty())
        {
            for (std::size_t boneIndex = 0; boneIndex < mesh.model->bones.size(); ++boneIndex)
            {
                const float wiggle = std::sin(animation.playbackTime + static_cast<float>(boneIndex) * 0.35f) * 0.2f;
                glm::mat4 transform = glm::rotate(glm::mat4(1.0f), wiggle, glm::vec3(0.0f, 1.0f, 0.0f));
                animation.boneMatrices[boneIndex] = transform * mesh.model->bones[boneIndex].offset;
            }
            continue;
        }

        const AnimationClip& clip = mesh.model->animations.front();
        if (clip.durationTicks <= 0.0)
        {
            continue;
        }

        const double ticksPerSecond = clip.ticksPerSecond > 0.0 ? clip.ticksPerSecond : 25.0;
        const double timeInTicks = std::fmod(static_cast<double>(animation.playbackTime) * ticksPerSecond, clip.durationTicks);

        for (const BoneAnimationChannel& channel : clip.channels)
        {
            if (channel.boneIndex < 0 || static_cast<std::size_t>(channel.boneIndex) >= animation.boneMatrices.size())
            {
                continue;
            }

            const glm::vec3 translation = SamplePosition(channel, timeInTicks);
            const glm::quat rotation = SampleRotation(channel, timeInTicks);
            const glm::vec3 scale = SampleScale(channel, timeInTicks);

            const glm::mat4 local = glm::translate(glm::mat4(1.0f), translation)
                * glm::toMat4(rotation)
                * glm::scale(glm::mat4(1.0f), scale);

            animation.boneMatrices[channel.boneIndex] = local * mesh.model->bones[channel.boneIndex].offset;
        }
    }
}
