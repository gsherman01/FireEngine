#include "Core/Input.h"
#include "Engine/EditorUI.h"
#include "Engine/Engine.h"
#include "Engine/SceneSerializer.h"
#include "Scene/Components.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef FIREENGINE_ENABLE_IMGUI
#include "./imgui.h"
#include "./imgui_impl_glfw.h"
#include "./imgui_impl_opengl3.h"
#endif

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

#ifdef FIREENGINE_ENABLE_IMGUI
    void DrawEditor(Engine& engine, const EditorUI& ui)
    {
        Registry& registry = engine.GetRegistry();
        static char assetImportPath[260] = "";
        static std::string assetImportStatus;

        ImGui::Begin("Scene Hierarchy");
        ui.DrawBanner("FireEngine", "Drag controls for transform editing");

        static EntityId selectedEntityId = 0;
        for (Entity entity : registry.GetEntities())
        {
            const NameComponent* name = entity.HasComponent<NameComponent>() ? &entity.GetComponent<NameComponent>() : nullptr;
            const std::string label = name != nullptr ? name->name : ("Entity " + std::to_string(entity.GetId()));

            const bool selected = (selectedEntityId == entity.GetId());
            if (ImGui::Selectable(("##sel" + std::to_string(entity.GetId())).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
            {
                selectedEntityId = entity.GetId();
            }
            ImGui::SameLine();
            const bool open = ImGui::TreeNode(reinterpret_cast<void*>(static_cast<intptr_t>(entity.GetId())), "%s", label.c_str());
            ImGui::SameLine();
            if (ImGui::SmallButton(("Delete##" + std::to_string(entity.GetId())).c_str()))
            {
                registry.DestroyEntity(entity);
                if (open) ImGui::TreePop();
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
                    ui.DrawDragVec3(("Position##" + std::to_string(entity.GetId())), &transform.transform.position.x, 0.05f);
                    ui.DrawDragVec3(("Rotation##" + std::to_string(entity.GetId())), &transform.transform.rotationDegrees.x, 0.25f);
                    ui.DrawDragVec3(("Scale##" + std::to_string(entity.GetId())), &transform.transform.scale.x, 0.01f);
                }
                if (entity.HasComponent<MaterialComponent>())
                {
                    MaterialComponent& matComp = entity.GetComponent<MaterialComponent>();
                    Material& mat = matComp.overrideMaterial;
                    ImGui::Checkbox(("Use Model Material##" + std::to_string(entity.GetId())).c_str(), &matComp.useModelMaterial);
                    ImGui::ColorEdit3(("Albedo##" + std::to_string(entity.GetId())).c_str(), &mat.albedo.x);
                    ImGui::SliderFloat(("Metallic##" + std::to_string(entity.GetId())).c_str(), &mat.metallic, 0.0f, 1.0f);
                    ImGui::SliderFloat(("Roughness##" + std::to_string(entity.GetId())).c_str(), &mat.roughness, 0.02f, 1.0f);
                    ImGui::SliderFloat(("AO##" + std::to_string(entity.GetId())).c_str(), &mat.ao, 0.0f, 2.0f);
                    ImGui::ColorEdit3(("Emissive##" + std::to_string(entity.GetId())).c_str(), &mat.emissive.x);
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
        static char scenePath[260] = "assets/scenes/default.fescene";
        ImGui::InputText("Scene File", scenePath, sizeof(scenePath));
        if (ImGui::Button("Save Scene"))
        {
            assetImportStatus = SceneSerializer::Save(registry, scenePath) ? "Scene saved." : "Scene save failed.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Scene"))
        {
            assetImportStatus = SceneSerializer::Load(registry, scenePath) ? "Scene loaded." : "Scene load failed.";
        }
        const AssetManager& assets = engine.GetAssetManager();
        ImGui::Text("Textures: %zu", assets.GetTextureCount());
        ImGui::Text("Models: %zu", assets.GetModelCount());
        ImGui::End();

        ImGui::Begin("Viewport");
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        engine.RenderEditor(static_cast<int>(viewportSize.x), static_cast<int>(viewportSize.y));
        static int gizmoMode = 0; // 0 move xz, 1 rotate y, 2 scale uniform
        ImGui::SeparatorText("Gizmo");
        ImGui::RadioButton("Move XZ", &gizmoMode, 0); ImGui::SameLine();
        ImGui::RadioButton("Rotate Y", &gizmoMode, 1); ImGui::SameLine();
        ImGui::RadioButton("Scale", &gizmoMode, 2);

        if (selectedEntityId != 0 && registry.HasEntity(selectedEntityId) && registry.HasComponent<TransformComponent>(selectedEntityId))
        {
            TransformComponent& selTransform = registry.GetComponent<TransformComponent>(selectedEntityId);
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !ImGui::IsAnyItemActive())
            {
                const ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
                if (gizmoMode == 0)
                {
                    selTransform.transform.position.x += drag.x * 0.01f;
                    selTransform.transform.position.z += drag.y * 0.01f;
                }
                else if (gizmoMode == 1)
                {
                    selTransform.transform.rotationDegrees.y += drag.x * 0.25f;
                }
                else
                {
                    const float s = 1.0f + (drag.x - drag.y) * 0.0025f;
                    selTransform.transform.scale *= glm::vec3(glm::max(0.1f, s));
                }
            }
        }

        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(engine.GetViewportTexture())), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        ImGui::End();
    }
#endif
}

int main(int argc, char** argv)
{
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const bool editorMode = IsEditorMode(argc, argv);
    EditorUI editorUi(editorMode);
    const char* title = editorMode ? "FireEngine - Editor Mode" : "FireEngine - Game Mode";
    GLFWwindow* window = glfwCreateWindow(kWindowWidth, kWindowHeight, title, nullptr, nullptr);
    if (window == nullptr) return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) return 1;
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glEnable(GL_DEPTH_TEST);
    Input::Initialize(window);

#ifdef FIREENGINE_ENABLE_IMGUI
    if (editorMode)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }
#endif

    Engine engine;
    if (!engine.Initialize(kWindowWidth, kWindowHeight)) return 1;
    float lastTime = static_cast<float>(glfwGetTime());

    // Main render loop:
    // 1) gather input/events
    // 2) update simulation/systems
    // 3) render either editor composited view or direct game view
    // 4) present backbuffer
    while (!glfwWindowShouldClose(window))
    {
        Input::BeginFrame();
        glfwPollEvents();
        if (Input::IsKeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);

        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        // Update camera + animation + other runtime systems before issuing draw calls.
        engine.Update(deltaTime);

#ifdef FIREENGINE_ENABLE_IMGUI
        if (editorMode)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // Editor path: render scene to offscreen framebuffer and composite via ImGui.
            DrawEditor(engine, editorUi);
            ImGui::Render();

            int windowWidth = 0, windowHeight = 0;
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowWidth, windowHeight);
            glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        else
#endif
        {
            int windowWidth = 0, windowHeight = 0;
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            // Game path: render scene directly to the window framebuffer.
            engine.RenderGame(windowWidth, windowHeight);
        }
        glfwSwapBuffers(window);
    }

#ifdef FIREENGINE_ENABLE_IMGUI
    if (editorMode)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
#endif

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
