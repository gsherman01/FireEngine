#pragma once

#include "Assets/AssetImporter.h"
#include "Assets/AssetManager.h"
#include "ECS/Registry.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/ShadowMap.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/RenderSystem.h"

#include <string>

class Engine
{
public:
    bool Initialize(int width, int height);
    void Update(float deltaTime);
    void RenderEditor(int viewportWidth, int viewportHeight);
    void RenderGame(int viewportWidth, int viewportHeight);
    bool ImportAsset(const std::string& path);

    unsigned int GetViewportTexture() const { return m_framebuffer.GetColorTexture(); }
    Registry& GetRegistry() { return m_registry; }
    const AssetManager& GetAssetManager() const { return m_assetManager; }

private:
    void BuildDemoScene();

    Registry m_registry;

    Renderer m_renderer;
    Shader m_shader;
    Shader m_shadowDepthShader;
    Shader m_skyboxShader;
    Framebuffer m_framebuffer;
    ShadowMap m_shadowMap;

    AssetManager m_assetManager;

    unsigned int m_skyboxTexture = 0;
};
