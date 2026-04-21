#pragma once

#include <glm/glm.hpp>
#include <vector>

class Registry;
class Renderer;
class Shader;

class RenderSystem
{
public:
    void RenderShadowMap(Registry& registry, const Renderer& renderer, const Shader& depthShader, const glm::mat4& lightSpaceMatrix) const;
    void RenderScene(
        Registry& registry,
        const Renderer& renderer,
        const Shader& shader,
        int viewportWidth,
        int viewportHeight,
        const glm::mat4& lightSpaceMatrix,
        unsigned int shadowMapTexture) const;

private:
    glm::vec3 ResolveDirectionalLightColor(Registry& registry) const;
    glm::vec3 ResolveDirectionalLightDirection(Registry& registry) const;
    glm::vec3 ResolvePointLightPosition(Registry& registry) const;
    glm::vec3 ResolvePointLightColor(Registry& registry) const;
};
