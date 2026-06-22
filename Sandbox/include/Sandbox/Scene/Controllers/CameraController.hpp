#pragma once

#include "Acidum/Scene/Camera.hpp"

namespace Sandbox {

class CameraController {
public:
    CameraController(Acidum::Camera& camera);

    void onUpdate(float deltaTime);
private:
    Acidum::Camera& m_camera;

    bool m_firstMouse = true;

    glm::vec2 m_lastMousePos { 0.0f, 0.0f };
    glm::vec3 m_cameraPos { 0.0f, 0.0f, 3.0f };

    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
};

} // namespace Sandbox