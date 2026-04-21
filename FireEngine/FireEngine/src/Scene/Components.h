#pragma once

#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Scene/Transform.h"

#include <glm/glm.hpp>
#include <string>

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
    Mesh* mesh = nullptr;
};

struct CameraComponent
{
    Camera camera;
    bool primary = false;
};

struct MaterialComponent
{
    glm::vec3 albedo = glm::vec3(1.0f);
    Texture* diffuseTexture = nullptr;
    Texture* specularTexture = nullptr;
    float shininess = 32.0f;
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
};

struct AnimationComponent
{
    float playbackTime = 0.0f;
};
