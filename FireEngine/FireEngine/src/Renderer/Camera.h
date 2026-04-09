#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    void ProcessKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime);
    void ProcessMouse(float deltaX, float deltaY);

    const glm::vec3& GetPosition() const { return m_position; }

private:
    void UpdateVectors();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;
    float m_moveSpeed;
    float m_mouseSensitivity;
    float m_fov;
};
