#include "Core/Input.h"
#include "Engine/Engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>

namespace
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;

    void FramebufferSizeCallback(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(kWindowWidth, kWindowHeight, "FireEngine - Milestone 1.5", nullptr, nullptr);
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Engine engine;
    if (!engine.Initialize(kWindowWidth, kWindowHeight))
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Scene Hierarchy");
        ImGui::TextUnformatted("- Camera");
        ImGui::TextUnformatted("- Directional Light");
        ImGui::TextUnformatted("- Point Light");
        ImGui::TextUnformatted("- Cube A");
        ImGui::TextUnformatted("- Cube B");
        ImGui::End();

        ImGui::Begin("Viewport");
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        engine.Render(static_cast<int>(viewportSize.x), static_cast<int>(viewportSize.y));

        ImGui::Image(
            reinterpret_cast<ImTextureID>(static_cast<intptr_t>(engine.GetViewportTexture())),
            viewportSize,
            ImVec2(0.0f, 1.0f),
            ImVec2(1.0f, 0.0f));
        ImGui::End();

        ImGui::Render();

        int windowWidth = 0;
        int windowHeight = 0;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
