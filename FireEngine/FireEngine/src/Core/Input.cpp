#include "Input.h"

#include <GLFW/glfw3.h>

GLFWwindow* Input::s_window = nullptr;
bool Input::s_initialized = false;
bool Input::s_firstMouse = true;
bool Input::s_keys[512] = {};
bool Input::s_mouseButtons[16] = {};
float Input::s_mouseX = 0.0f;
float Input::s_mouseY = 0.0f;
float Input::s_mouseDeltaX = 0.0f;
float Input::s_mouseDeltaY = 0.0f;

void Input::Initialize(GLFWwindow* window)
{
    s_window = window;
    s_initialized = true;
    s_firstMouse = true;

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
}

void Input::BeginFrame()
{
    s_mouseDeltaX = 0.0f;
    s_mouseDeltaY = 0.0f;
}

bool Input::IsKeyPressed(int key)
{
    if (!s_initialized || key < 0 || key >= 512)
    {
        return false;
    }

    return s_keys[key];
}

bool Input::IsMouseButtonPressed(int button)
{
    if (!s_initialized || button < 0 || button >= 16)
    {
        return false;
    }

    return s_mouseButtons[button];
}

float Input::GetMouseX()
{
    return s_mouseX;
}

float Input::GetMouseY()
{
    return s_mouseY;
}

float Input::GetMouseDeltaX()
{
    return s_mouseDeltaX;
}

float Input::GetMouseDeltaY()
{
    return s_mouseDeltaY;
}

void Input::KeyCallback(GLFWwindow*, int key, int, int action, int)
{
    if (key < 0 || key >= 512)
    {
        return;
    }

    if (action == GLFW_PRESS)
    {
        s_keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        s_keys[key] = false;
    }
}

void Input::MouseButtonCallback(GLFWwindow*, int button, int action, int)
{
    if (button < 0 || button >= 16)
    {
        return;
    }

    if (action == GLFW_PRESS)
    {
        s_mouseButtons[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        s_mouseButtons[button] = false;
    }
}

void Input::CursorPositionCallback(GLFWwindow*, double xPos, double yPos)
{
    if (s_firstMouse)
    {
        s_mouseX = static_cast<float>(xPos);
        s_mouseY = static_cast<float>(yPos);
        s_firstMouse = false;
        return;
    }

    const float newX = static_cast<float>(xPos);
    const float newY = static_cast<float>(yPos);

    s_mouseDeltaX += (newX - s_mouseX);
    s_mouseDeltaY += (s_mouseY - newY);

    s_mouseX = newX;
    s_mouseY = newY;
}
