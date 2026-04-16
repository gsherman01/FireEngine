#pragma once

struct GLFWwindow;

class Input
{
public:
    static void Initialize(GLFWwindow* window);
    static void BeginFrame();

    static bool IsKeyPressed(int key);
    static bool IsMouseButtonPressed(int button);

    static float GetMouseX();
    static float GetMouseY();
    static float GetMouseDeltaX();
    static float GetMouseDeltaY();

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPositionCallback(GLFWwindow* window, double xPos, double yPos);

    static GLFWwindow* s_window;
    static bool s_initialized;
    static bool s_firstMouse;
    static bool s_keys[512];
    static bool s_mouseButtons[16];
    static float s_mouseX;
    static float s_mouseY;
    static float s_mouseDeltaX;
    static float s_mouseDeltaY;
};
