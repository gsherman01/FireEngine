#pragma once

#include "Renderer/Renderer.h"

#include <glm/glm.hpp>

#include <vector>

class Registry;
class Shader;

class RenderSystem
{
public:
    void RenderShadowMap(Registry& registry, const Renderer& renderer, const Shader& depthShader, const glm::mat4& lightSpaceMatrix) const;
    void RenderScene(
        Registry& registry,
        const Renderer& renderer,
        const Shader& fallbackShader,
        const Shader& skyboxShader,
        int viewportWidth,
        int viewportHeight,
        const glm::mat4& lightSpaceMatrix,
        unsigned int shadowMapTexture,
        unsigned int skyboxTexture) const;

private:
    std::vector<RenderLight> CollectLights(Registry& registry) const;
};
