#pragma once

#include "ECS/Registry.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/RenderSystem.h"

class Engine
{
public:
    bool Initialize(int width, int height);
    void Update(float deltaTime);
    void Render(int viewportWidth, int viewportHeight);

    unsigned int GetViewportTexture() const { return m_framebuffer.GetColorTexture(); }

private:
    void BuildDemoScene();

    Registry m_registry;

    Renderer m_renderer;
    Shader m_shader;
    Framebuffer m_framebuffer;
    Mesh m_cubeMesh;

    CameraSystem m_cameraSystem;
    RenderSystem m_renderSystem;
    AnimationSystem m_animationSystem;
};
