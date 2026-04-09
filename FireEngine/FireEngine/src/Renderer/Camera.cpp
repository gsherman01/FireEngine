#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : m_position(0.0f, 0.0f, 3.0f),
      m_front(0.0f, 0.0f, -1.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_right(1.0f, 0.0f, 0.0f),
      m_worldUp(0.0f, 1.0f, 0.0f),
      m_yaw(-90.0f),
      m_pitch(0.0f),
      m_moveSpeed(5.0f),
      m_mouseSensitivity(0.08f),
      m_fov(60.0f)
{
    UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::ProcessKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime)
{
    const float velocity = m_moveSpeed * deltaTime;

    if (forward)
    {
        m_position += m_front * velocity;
    }
    if (backward)
    {
        m_position -= m_front * velocity;
    }
    if (left)
    {
        m_position -= m_right * velocity;
    }
    if (right)
    {
        m_position += m_right * velocity;
    }
}

void Camera::ProcessMouse(float deltaX, float deltaY)
{
    m_yaw += deltaX * m_mouseSensitivity;
    m_pitch += deltaY * m_mouseSensitivity;

    if (m_pitch > 89.0f)
    {
        m_pitch = 89.0f;
    }
    if (m_pitch < -89.0f)
    {
        m_pitch = -89.0f;
    }

    UpdateVectors();
}

void Camera::UpdateVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
