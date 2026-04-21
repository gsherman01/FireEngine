#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Scene/Components.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

namespace
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;

    void FramebufferSizeCallback(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    bool IsEditorMode(int argc, char** argv)
    {
        for (int i = 1; i < argc; ++i)
        {
            if (std::strcmp(argv[i], "--game") == 0)
            {
                return false;
            }
        }

        return true;
    }

    void DrawEditor(Engine& engine)
    {
        Registry& registry = engine.GetRegistry();
        static char assetImportPath[260] = "";
        static std::string assetImportStatus;

        ImGui::Begin("Scene Hierarchy");
        for (Entity entity : registry.GetEntities())
        {
            const NameComponent* name = entity.HasComponent<NameComponent>() ? &entity.GetComponent<NameComponent>() : nullptr;
            const std::string label = name != nullptr ? name->name : ("Entity " + std::to_string(entity.GetId()));

            const bool open = ImGui::TreeNode(reinterpret_cast<void*>(static_cast<intptr_t>(entity.GetId())), "%s", label.c_str());
            ImGui::SameLine();
            if (ImGui::SmallButton(("Delete##" + std::to_string(entity.GetId())).c_str()))
            {
                registry.DestroyEntity(entity);
                if (open)
                {
                    ImGui::TreePop();
                }
                break;
            }

            if (open)
            {
                if (name != nullptr)
                {
                    char nameBuffer[128]{};
                    std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", name->name.c_str());
                    if (ImGui::InputText(("Name##" + std::to_string(entity.GetId())).c_str(), nameBuffer, sizeof(nameBuffer)))
                    {
                        name->name = nameBuffer;
                    }
                }

                if (entity.HasComponent<TransformComponent>())
                {
                    TransformComponent& transform = entity.GetComponent<TransformComponent>();
                    ImGui::InputFloat3(("Position##" + std::to_string(entity.GetId())).c_str(), &transform.transform.position.x);
                    ImGui::InputFloat3(("Rotation##" + std::to_string(entity.GetId())).c_str(), &transform.transform.rotationDegrees.x);
                    ImGui::InputFloat3(("Scale##" + std::to_string(entity.GetId())).c_str(), &transform.transform.scale.x);
                }

                ImGui::TreePop();
            }
        }

        if (ImGui::Button("Create Empty Entity"))
        {
            Entity entity = registry.CreateEntity();
            entity.AddComponent<NameComponent>().name = "New Entity";
            entity.AddComponent<TransformComponent>();
        }

        ImGui::End();

        ImGui::Begin("Asset Browser");
        ImGui::InputText("Path", assetImportPath, sizeof(assetImportPath));
        ImGui::SameLine();
        if (ImGui::Button("Import"))
        {
            assetImportStatus = engine.ImportAsset(assetImportPath) ? "Imported successfully." : "Import failed.";
        }
        ImGui::TextUnformatted(assetImportStatus.c_str());

        const AssetManager& assets = engine.GetAssetManager();
        ImGui::Text("Textures: %zu", assets.GetTextureCount());
        ImGui::Text("Audio: %zu", assets.GetAudioCount());
        ImGui::Text("Meshes: %zu", assets.GetMeshCount());
        ImGui::Text("Animations: %zu", assets.GetAnimationCount());
        ImGui::End();

        ImGui::Begin("Viewport");
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        engine.RenderEditor(static_cast<int>(viewportSize.x), static_cast<int>(viewportSize.y));

        ImGui::Image(
            reinterpret_cast<ImTextureID>(static_cast<intptr_t>(engine.GetViewportTexture())),
            viewportSize,
            ImVec2(0.0f, 1.0f),
            ImVec2(1.0f, 0.0f));
        ImGui::End();
    }
}

int main(int argc, char** argv)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const bool editorMode = IsEditorMode(argc, argv);
    const char* title = editorMode ? "FireEngine - Editor Mode" : "FireEngine - Game Mode";
    GLFWwindow* window = glfwCreateWindow(kWindowWidth, kWindowHeight, title, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glEnable(GL_DEPTH_TEST);

    Input::Initialize(window);

    if (editorMode)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    Engine engine;
    if (!engine.Initialize(kWindowWidth, kWindowHeight))
    {
        if (editorMode)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window))
    {
        Input::BeginFrame();
        glfwPollEvents();

        if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        engine.Update(deltaTime);

        if (editorMode)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            DrawEditor(engine);

            ImGui::Render();

            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowWidth, windowHeight);
            glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        else
        {
            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            engine.RenderGame(windowWidth, windowHeight);
        }

        glfwSwapBuffers(window);
    }

    if (editorMode)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
