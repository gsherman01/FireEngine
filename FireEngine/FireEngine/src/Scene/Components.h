#pragma once

#include "Renderer/Camera.h"
#include "Renderer/Material.h"
#include "Renderer/Model.h"
#include "Scene/Transform.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct NameComponent
{
    std::string name = "Entity";
};

struct TransformComponent
{
    Transform transform;
};

struct MeshComponent
{
    std::shared_ptr<Model> model;
};

struct CameraComponent
{
    Camera camera;
    bool primary = false;
};

struct MaterialComponent
{
    Material overrideMaterial;
    bool useModelMaterial = true;
};

enum class LightType
{
    Directional,
    Point
};

struct LightComponent
{
    LightType type = LightType::Point;
    glm::vec3 color = glm::vec3(1.0f);
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 position = glm::vec3(2.0f, 2.0f, 2.0f);
    float intensity = 1.0f;
    float range = 15.0f;
};

struct AnimationComponent
{
    float playbackTime = 0.0f;
    float speed = 1.0f;
    std::vector<glm::mat4> boneMatrices;
};
