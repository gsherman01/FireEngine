#include "Engine.h"

#include "Scene/Components.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>
#include <algorithm>
#include <cctype>
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

    glm::mat4 BuildLightSpaceMatrix(Registry& registry)
    {
        glm::vec3 lightDirection(-0.5f, -1.0f, -0.35f);
        for (Entity lightEntity : registry.View<LightComponent>())
        {
            const LightComponent& light = lightEntity.GetComponent<LightComponent>();
            if (light.type == LightType::Directional)
            {
                lightDirection = glm::normalize(light.direction);
                break;
            }
        }

        const glm::vec3 lightPosition = -lightDirection * 8.0f;
        const glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0, 1, 0));
        const glm::mat4 lightProjection = glm::ortho(-12.0f, 12.0f, -12.0f, 12.0f, 1.0f, 30.0f);
        return lightProjection * lightView;
    }
}

bool Engine::Initialize(int width, int height)
{
    if (!m_shader.LoadFromFiles("assets/shaders/basic.vert", "assets/shaders/basic.frag"))
    {
        return false;
    }

    if (!m_shadowDepthShader.LoadFromFiles("assets/shaders/shadow_depth.vert", "assets/shaders/shadow_depth.frag"))
    {
        return false;
    }

    m_cubeMesh.SetData(BuildCubeVertices(), BuildCubeIndices());

    if (!m_framebuffer.Create(width, height))
    {
        return false;
    }
    if (!m_shadowMap.Create(2048))
    {
        return false;
    }

    BuildDemoScene();
    m_registry.RegisterSystem<CameraSystem>();
    m_registry.SetSystemSignature<CameraSystem, CameraComponent>();
    m_registry.RegisterSystem<AnimationSystem>();
    m_registry.SetSystemSignature<AnimationSystem, AnimationComponent, TransformComponent>();
    m_registry.RegisterSystem<RenderSystem>();
    m_registry.SetSystemSignature<RenderSystem, TransformComponent, MeshComponent, MaterialComponent>();
    return true;
}

void Engine::Update(float deltaTime)
{
    m_registry.GetSystem<CameraSystem>().Update(m_registry, deltaTime);
    m_registry.GetSystem<AnimationSystem>().Update(m_registry, deltaTime);
}

bool Engine::ImportAsset(const std::string& path)
{
    const std::filesystem::path assetPath(path);
    if (!std::filesystem::exists(assetPath))
    {
        return false;
    }

    std::string extension = assetPath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (extension == ".png")
    {
        return m_assetImporter.ImportTexturePng(assetPath, m_assetManager).has_value();
    }

    if (extension == ".wav" || extension == ".mp3" || extension == ".ogg")
    {
        return m_assetImporter.ImportAudio(assetPath, m_assetManager).has_value();
    }

    return m_assetImporter.ImportMeshAndAnimations(assetPath, m_assetManager).has_value();
}

void Engine::RenderEditor(int viewportWidth, int viewportHeight)
{
    if (viewportWidth <= 0 || viewportHeight <= 0)
    {
        return;
    }

    m_framebuffer.Resize(viewportWidth, viewportHeight);
    const glm::mat4 lightSpaceMatrix = BuildLightSpaceMatrix(m_registry);

    m_shadowMap.BindForWriting();
    m_registry.GetSystem<RenderSystem>().RenderShadowMap(m_registry, m_renderer, m_shadowDepthShader, lightSpaceMatrix);

    m_framebuffer.Bind();
    glViewport(0, 0, m_framebuffer.GetWidth(), m_framebuffer.GetHeight());

    m_renderer.BeginFrame({0.08f, 0.10f, 0.14f});
    m_registry.GetSystem<RenderSystem>().RenderScene(
        m_registry,
        m_renderer,
        m_shader,
        m_framebuffer.GetWidth(),
        m_framebuffer.GetHeight(),
        lightSpaceMatrix,
        m_shadowMap.GetDepthTexture());

    m_framebuffer.Unbind();
}

void Engine::RenderGame(int viewportWidth, int viewportHeight)
{
    if (viewportWidth <= 0 || viewportHeight <= 0)
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
    const glm::mat4 lightSpaceMatrix = BuildLightSpaceMatrix(m_registry);
    m_shadowMap.BindForWriting();
    m_registry.GetSystem<RenderSystem>().RenderShadowMap(m_registry, m_renderer, m_shadowDepthShader, lightSpaceMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
    m_renderer.BeginFrame({0.08f, 0.10f, 0.14f});
    m_registry.GetSystem<RenderSystem>().RenderScene(
        m_registry,
        m_renderer,
        m_shader,
        viewportWidth,
        viewportHeight,
        lightSpaceMatrix,
        m_shadowMap.GetDepthTexture());
}

void Engine::BuildDemoScene()
{
    Entity cameraEntity = m_registry.CreateEntity();
    cameraEntity.AddComponent<NameComponent>().name = "Camera";
    CameraComponent& camera = cameraEntity.AddComponent<CameraComponent>();
    camera.primary = true;

    Entity directionalLight = m_registry.CreateEntity();
    directionalLight.AddComponent<NameComponent>().name = "Directional Light";
    LightComponent& dirLight = directionalLight.AddComponent<LightComponent>();
    dirLight.type = LightType::Directional;
    dirLight.direction = {-0.5f, -1.0f, -0.35f};
    dirLight.color = {0.85f, 0.85f, 0.90f};
    dirLight.intensity = 0.7f;

    Entity pointLight = m_registry.CreateEntity();
    pointLight.AddComponent<NameComponent>().name = "Point Light";
    LightComponent& pLight = pointLight.AddComponent<LightComponent>();
    pLight.type = LightType::Point;
    pLight.position = {2.0f, 2.0f, 2.0f};
    pLight.color = {1.0f, 1.0f, 1.0f};
    pLight.intensity = 1.0f;

    Entity cubeA = m_registry.CreateEntity();
    cubeA.AddComponent<NameComponent>().name = "Cube A";
    TransformComponent& transformA = cubeA.AddComponent<TransformComponent>();
    transformA.transform.position = {-1.25f, 0.0f, 0.0f};
    cubeA.AddComponent<MeshComponent>().mesh = &m_cubeMesh;
    cubeA.AddComponent<MaterialComponent>().albedo = {0.95f, 0.40f, 0.35f};

    Entity cubeB = m_registry.CreateEntity();
    cubeB.AddComponent<NameComponent>().name = "Cube B";
    TransformComponent& transformB = cubeB.AddComponent<TransformComponent>();
    transformB.transform.position = {1.25f, 0.0f, 0.0f};
    cubeB.AddComponent<MeshComponent>().mesh = &m_cubeMesh;
    cubeB.AddComponent<MaterialComponent>().albedo = {0.30f, 0.65f, 0.95f};
    cubeB.AddComponent<AnimationComponent>();
}
