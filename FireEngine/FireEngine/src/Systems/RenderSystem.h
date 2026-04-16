#pragma once

#include <glm/glm.hpp>

class Registry;
class Renderer;
class Shader;

class RenderSystem
{
public:
    void RenderScene(Registry& registry, const Renderer& renderer, const Shader& shader, int viewportWidth, int viewportHeight) const;

private:
    glm::vec3 ResolveDirectionalLightColor(Registry& registry) const;
    glm::vec3 ResolveDirectionalLightDirection(Registry& registry) const;
    glm::vec3 ResolvePointLightPosition(Registry& registry) const;
    glm::vec3 ResolvePointLightColor(Registry& registry) const;
};
