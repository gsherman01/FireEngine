#pragma once

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct BoneInfo
{
    glm::mat4 offset = glm::mat4(1.0f);
    glm::mat4 finalTransform = glm::mat4(1.0f);
};

struct ModelMesh
{
    std::shared_ptr<Mesh> mesh;
    std::size_t materialIndex = 0;
};

struct PositionKey
{
    double time = 0.0;
    glm::vec3 value = glm::vec3(0.0f);
};

struct RotationKey
{
    double time = 0.0;
    glm::quat value = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

struct ScaleKey
{
    double time = 0.0;
    glm::vec3 value = glm::vec3(1.0f);
};

struct BoneAnimationChannel
{
    int boneIndex = -1;
    std::vector<PositionKey> positions;
    std::vector<RotationKey> rotations;
    std::vector<ScaleKey> scales;
};

struct AnimationClip
{
    std::string name;
    double durationTicks = 0.0;
    double ticksPerSecond = 25.0;
    std::vector<BoneAnimationChannel> channels;
};

class Model
{
public:
    std::string sourcePath;
    std::vector<ModelMesh> meshes;
    std::vector<Material> materials;

    std::unordered_map<std::string, int> boneNameToIndex;
    std::vector<BoneInfo> bones;
    std::vector<AnimationClip> animations;

    bool HasBones() const { return !bones.empty(); }
    bool HasAnimations() const { return !animations.empty(); }
};
