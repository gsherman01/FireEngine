#include "Renderer/Camera.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Scene/Transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

namespace
{
    int g_windowWidth = 1280;
    int g_windowHeight = 720;
    bool g_firstMouse = true;
    float g_lastMouseX = 0.0f;
    float g_lastMouseY = 0.0f;
    Camera* g_camera = nullptr;
    Framebuffer* g_framebuffer = nullptr;

    void FramebufferSizeCallback(GLFWwindow*, int width, int height)
    {
        g_windowWidth = width;
        g_windowHeight = height;
        glViewport(0, 0, width, height);

        if (g_framebuffer != nullptr)
        {
            g_framebuffer->Resize(width, height);
        }
    }

    void MouseCallback(GLFWwindow*, double xPos, double yPos)
    {
        if (g_firstMouse)
        {
            g_lastMouseX = static_cast<float>(xPos);
            g_lastMouseY = static_cast<float>(yPos);
            g_firstMouse = false;
        }

        const float deltaX = static_cast<float>(xPos) - g_lastMouseX;
        const float deltaY = g_lastMouseY - static_cast<float>(yPos);

        g_lastMouseX = static_cast<float>(xPos);
        g_lastMouseY = static_cast<float>(yPos);

        if (g_camera != nullptr)
        {
            g_camera->ProcessMouse(deltaX, deltaY);
        }
    }

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

    GLFWwindow* window = glfwCreateWindow(g_windowWidth, g_windowHeight, "FireEngine - Milestone 1", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create window." << std::endl;
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
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Shader shader;
    if (!shader.LoadFromFiles("assets/shaders/basic.vert", "assets/shaders/basic.frag"))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    Mesh cube;
    cube.SetData(BuildCubeVertices(), BuildCubeIndices());

    Camera camera;
    g_camera = &camera;

    Framebuffer framebuffer;
    if (!framebuffer.Create(g_windowWidth, g_windowHeight))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    g_framebuffer = &framebuffer;

    Renderer renderer;

    Transform objectA;
    objectA.position = {-1.25f, 0.0f, 0.0f};

    Transform objectB;
    objectB.position = {1.25f, 0.0f, 0.0f};

    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window))
    {
        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        camera.ProcessKeyboard(
            glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS,
            glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS,
            glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS,
            glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS,
            deltaTime);

        objectA.rotationDegrees.y += 25.0f * deltaTime;
        objectB.rotationDegrees.x += 30.0f * deltaTime;

        framebuffer.Bind();
        glViewport(0, 0, framebuffer.GetWidth(), framebuffer.GetHeight());

        renderer.BeginFrame({0.08f, 0.10f, 0.14f});

        const float aspect = static_cast<float>(framebuffer.GetWidth()) / static_cast<float>(framebuffer.GetHeight());
        const glm::vec3 lightPosition(2.0f, 2.0f, 2.0f);
        const glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

        renderer.DrawMesh(cube, shader, objectA.ToMatrix(), camera, aspect, {0.95f, 0.40f, 0.35f}, lightPosition, lightColor);
        renderer.DrawMesh(cube, shader, objectB.ToMatrix(), camera, aspect, {0.30f, 0.65f, 0.95f}, lightPosition, lightColor);

        Framebuffer::Unbind();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.GetHandle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, framebuffer.GetWidth(), framebuffer.GetHeight(),
            0, 0, g_windowWidth, g_windowHeight,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_framebuffer = nullptr;
    g_camera = nullptr;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
