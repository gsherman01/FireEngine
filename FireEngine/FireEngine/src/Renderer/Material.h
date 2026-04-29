#pragma once

#include "Renderer/Texture.h"

#include <glm/glm.hpp>

#include <memory>

class Shader;

struct Material
{
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> diffuseTexture;
    std::shared_ptr<Texture> specularTexture;
    std::shared_ptr<Texture> normalTexture;
    glm::vec3 albedo = glm::vec3(1.0f);
    float shininess = 32.0f;
    float metallic = 0.0f;
    float roughness = 0.6f;
    float ao = 1.0f;
    glm::vec3 emissive = glm::vec3(0.0f);
};
