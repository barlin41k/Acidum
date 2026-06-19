#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Acidum {

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearClip, float farClip);

    void setPosition(const glm::vec3& position);
    void setRotation(float pitch, float yaw);
    void setAspectRatio(float aspectRatio);

    const glm::vec3& getFront() const { return m_front; }
    const glm::vec3& getRight() const { return m_right; }
    const glm::vec3& getUp() const { return m_up; }

    const glm::mat4& getViewMatrix();
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
private:
    bool m_isViewMatrixDirty = true;

    glm::vec3 m_position{ 0.0f, 0.0f, 3.0f };
    
    float m_pitch = 0.0f;
    float m_yaw = -90.0f;
    float m_fov = 45.0f;
    float m_aspectRatio = 1.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 100.0f;

    glm::vec3 m_front { 0.0f, 0.0f, -1.0f };
    glm::vec3 m_right { 1.0f, 0.0f, 0.0f };
    glm::vec3 m_up { 0.0f, 1.0f, 0.0f };
    const glm::vec3 m_worldUp{ 0.0f, 1.0f, 0.0f };

    glm::mat4 m_viewMatrix { 1.0f };
    glm::mat4 m_projectionMatrix { 1.0f };

    void updateVectors();

    void recalculateViewMatrix();
    void recalculateProjectionMatrix();
};

} // namespace Acidum