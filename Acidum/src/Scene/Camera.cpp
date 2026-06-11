#include "Acidum/Scene/Camera.hpp"

namespace Acidum {

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_fov(fov), m_aspectRatio(aspectRatio), m_nearClip(nearClip), m_farClip(farClip)
{
    recalculateViewMatrix();
    recalculateProjectionMatrix();
}

void Camera::recalculateViewMatrix() {
    glm::vec3 front;

    double yawRad = glm::radians(m_yaw);
    double pitchRad = glm::radians(m_pitch);

    double cosPitch = cos(pitchRad);
    double sinPitch = sin(pitchRad);

    double sinYaw = sin(yawRad);
    double cosYaw = cos(yawRad);

    front.x = static_cast<float>(cosYaw * cosPitch);
    front.y = static_cast<float>(sinPitch);
    front.z = static_cast<float>(sinYaw * cosPitch);

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));

    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::recalculateProjectionMatrix() {
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
}

void Camera::setAspectRatio(float aspectRatio) {
    if (m_aspectRatio != aspectRatio) {
        m_aspectRatio = aspectRatio; 
        recalculateProjectionMatrix();
    }
}

} // namespace Acidum