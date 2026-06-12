#include "Acidum/Scene/Camera.hpp"

namespace Acidum {

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_fov(fov), m_aspectRatio(aspectRatio), m_nearClip(nearClip), m_farClip(farClip)
{
    updateVectors();
    recalculateViewMatrix();
    recalculateProjectionMatrix();
}

const glm::mat4& Camera::getViewMatrix() {
    if (m_isViewMatrixDirty) {
        recalculateViewMatrix();
        m_isViewMatrixDirty = false;
    }

    return m_viewMatrix;
}

void Camera::recalculateViewMatrix() {
    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::recalculateProjectionMatrix() {
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    m_isViewMatrixDirty = true;
}

void Camera::setRotation(float pitch, float yaw) {
    m_pitch = pitch;
    m_yaw = yaw;
    updateVectors();
    m_isViewMatrixDirty = true;
}

void Camera::setAspectRatio(float aspectRatio) {
    if (m_aspectRatio != aspectRatio) {
        m_aspectRatio = aspectRatio; 
        recalculateProjectionMatrix();
    }
}

void Camera::updateVectors() {
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
}

} // namespace Acidum