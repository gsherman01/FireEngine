#pragma once

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

#include <glm/glm.hpp>

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

class Model
{
public:
    std::string sourcePath;
    std::vector<ModelMesh> meshes;
    std::vector<Material> materials;

    std::unordered_map<std::string, int> boneNameToIndex;
    std::vector<BoneInfo> bones;

    bool HasBones() const { return !bones.empty(); }
};
