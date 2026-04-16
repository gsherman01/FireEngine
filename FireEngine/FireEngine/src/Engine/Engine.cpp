#include "Engine.h"

#include "Scene/Components.h"

#include <glad/glad.h>

#include <vector>

namespace
{
    std::vector<Vertex> BuildCubeVertices()
    {
        return {
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

            {{ 0.5f, -0.5f, -0.5f}, { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {0.0f,-1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f,-1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f,-1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.0f,-1.0f, 0.0f}, {0.0f, 1.0f}}
        };
    }

    std::vector<unsigned int> BuildCubeIndices()
    {
        return {
            0, 1, 2, 2, 3, 0,
            4, 6, 5, 6, 4, 7,
            8, 9,10,10,11, 8,
            12,14,13,14,12,15,
            16,17,18,18,19,16,
            20,22,21,22,20,23
        };
    }
}

bool Engine::Initialize(int width, int height)
{
    if (!m_shader.LoadFromFiles("assets/shaders/basic.vert", "assets/shaders/basic.frag"))
    {
        return false;
    }

    m_cubeMesh.SetData(BuildCubeVertices(), BuildCubeIndices());

    if (!m_framebuffer.Create(width, height))
    {
        return false;
    }

    BuildDemoScene();
    return true;
}

void Engine::Update(float deltaTime)
{
    m_cameraSystem.Update(m_registry, deltaTime);
    m_animationSystem.Update(m_registry, deltaTime);
}

void Engine::Render(int viewportWidth, int viewportHeight)
{
    if (viewportWidth <= 0 || viewportHeight <= 0)
    {
        return;
    }

    m_framebuffer.Resize(viewportWidth, viewportHeight);
    m_framebuffer.Bind();
    glViewport(0, 0, m_framebuffer.GetWidth(), m_framebuffer.GetHeight());

    m_renderer.BeginFrame({0.08f, 0.10f, 0.14f});
    m_renderSystem.RenderScene(m_registry, m_renderer, m_shader, m_framebuffer.GetWidth(), m_framebuffer.GetHeight());

    m_framebuffer.Unbind();
}

void Engine::BuildDemoScene()
{
    const Entity cameraEntity = m_registry.CreateEntity();
    CameraComponent& camera = m_registry.AddComponent<CameraComponent>(cameraEntity);
    camera.primary = true;

    const Entity directionalLight = m_registry.CreateEntity();
    LightComponent& dirLight = m_registry.AddComponent<LightComponent>(directionalLight);
    dirLight.type = LightType::Directional;
    dirLight.direction = {-0.5f, -1.0f, -0.35f};
    dirLight.color = {0.85f, 0.85f, 0.90f};
    dirLight.intensity = 0.7f;

    const Entity pointLight = m_registry.CreateEntity();
    LightComponent& pLight = m_registry.AddComponent<LightComponent>(pointLight);
    pLight.type = LightType::Point;
    pLight.position = {2.0f, 2.0f, 2.0f};
    pLight.color = {1.0f, 1.0f, 1.0f};
    pLight.intensity = 1.0f;

    const Entity cubeA = m_registry.CreateEntity();
    TransformComponent& transformA = m_registry.AddComponent<TransformComponent>(cubeA);
    transformA.transform.position = {-1.25f, 0.0f, 0.0f};
    m_registry.AddComponent<MeshComponent>(cubeA).mesh = &m_cubeMesh;
    m_registry.AddComponent<MaterialComponent>(cubeA).albedo = {0.95f, 0.40f, 0.35f};

    const Entity cubeB = m_registry.CreateEntity();
    TransformComponent& transformB = m_registry.AddComponent<TransformComponent>(cubeB);
    transformB.transform.position = {1.25f, 0.0f, 0.0f};
    m_registry.AddComponent<MeshComponent>(cubeB).mesh = &m_cubeMesh;
    m_registry.AddComponent<MaterialComponent>(cubeB).albedo = {0.30f, 0.65f, 0.95f};
    m_registry.AddComponent<AnimationComponent>(cubeB);
}
