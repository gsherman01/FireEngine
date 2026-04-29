#include "Engine.h"

#include "Scene/Components.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>
#include <vector>

namespace
{
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

    if (!m_skyboxShader.LoadFromFiles("assets/shaders/skybox.vert", "assets/shaders/skybox.frag"))
    {
        return false;
    }

    if (!m_framebuffer.Create(width, height) || !m_shadowMap.Create(2048))
    {
        return false;
    }

    auto skybox = m_assetManager.LoadCubemap("default_sky", {
        "assets/skybox/right.png",
        "assets/skybox/left.png",
        "assets/skybox/top.png",
        "assets/skybox/bottom.png",
        "assets/skybox/front.png",
        "assets/skybox/back.png"});
    if (skybox != nullptr)
    {
        m_skyboxTexture = skybox->GetId();
    }

    BuildDemoScene();
    m_registry.RegisterSystem<CameraSystem>();
    m_registry.SetSystemSignature<CameraSystem, CameraComponent>();
    m_registry.RegisterSystem<AnimationSystem>();
    m_registry.SetSystemSignature<AnimationSystem, AnimationComponent>();
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
    return m_assetManager.LoadModel(path) != nullptr || m_assetManager.LoadTexture(path) != nullptr;
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
        m_skyboxShader,
        m_framebuffer.GetWidth(),
        m_framebuffer.GetHeight(),
        lightSpaceMatrix,
        m_shadowMap.GetDepthTexture(),
        m_skyboxTexture);

    m_framebuffer.Unbind();
}

void Engine::RenderGame(int viewportWidth, int viewportHeight)
{
    if (viewportWidth <= 0 || viewportHeight <= 0)
    {
        return;
    }

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
        m_skyboxShader,
        viewportWidth,
        viewportHeight,
        lightSpaceMatrix,
        m_shadowMap.GetDepthTexture(),
        m_skyboxTexture);
}

void Engine::BuildDemoScene()
{
    Entity cameraEntity = m_registry.CreateEntity();
    cameraEntity.AddComponent<NameComponent>().name = "Camera";
    cameraEntity.AddComponent<TransformComponent>();
    cameraEntity.AddComponent<CameraComponent>().primary = true;

    Entity directionalLight = m_registry.CreateEntity();
    directionalLight.AddComponent<NameComponent>().name = "Directional Light";
    LightComponent& dirLight = directionalLight.AddComponent<LightComponent>();
    dirLight.type = LightType::Directional;
    dirLight.direction = {-0.5f, -1.0f, -0.35f};
    dirLight.color = {0.85f, 0.85f, 0.90f};
    dirLight.intensity = 0.7f;

    for (int i = 0; i < 3; ++i)
    {
        Entity pointLight = m_registry.CreateEntity();
        pointLight.AddComponent<NameComponent>().name = "Point Light " + std::to_string(i + 1);
        LightComponent& pLight = pointLight.AddComponent<LightComponent>();
        pLight.type = LightType::Point;
        pLight.position = {2.5f - i * 2.0f, 1.8f, 1.0f + i};
        pLight.color = {1.0f, 0.95f - 0.2f * i, 0.8f + 0.1f * i};
        pLight.intensity = 1.0f;
    }

    const std::vector<std::string> demoModels = {
        "assets/models/demo_model.obj",
        "assets/models/ogre/ogre.obj",
        "assets/models/dragon/dragon.obj"};

    std::shared_ptr<Model> model = nullptr;
    for (const std::string& modelPath : demoModels)
    {
        model = m_assetManager.LoadModel(modelPath);
        if (model != nullptr)
        {
            break;
        }
    }

    if (model == nullptr)
    {
        return;
    }

    Entity modelA = m_registry.CreateEntity();
    modelA.AddComponent<NameComponent>().name = "Model A";
    TransformComponent& transformA = modelA.AddComponent<TransformComponent>();
    transformA.transform.position = {-1.5f, 0.0f, 0.0f};
    transformA.transform.scale = {0.75f, 0.75f, 0.75f};
    modelA.AddComponent<MeshComponent>().model = model;
    modelA.AddComponent<MaterialComponent>();

    Entity modelB = m_registry.CreateEntity();
    modelB.AddComponent<NameComponent>().name = "Model B";
    TransformComponent& transformB = modelB.AddComponent<TransformComponent>();
    transformB.transform.position = {1.5f, 0.0f, 0.0f};
    transformB.transform.scale = {0.75f, 0.75f, 0.75f};
    modelB.AddComponent<MeshComponent>().model = model;

    MaterialComponent& material = modelB.AddComponent<MaterialComponent>();
    material.useModelMaterial = false;
    material.overrideMaterial.albedo = {0.35f, 0.85f, 0.95f};
    material.overrideMaterial.shininess = 48.0f;
    modelB.AddComponent<AnimationComponent>();
}
