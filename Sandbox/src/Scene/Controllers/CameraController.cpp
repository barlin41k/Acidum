#include "Sandbox/Scene/Controllers/CameraController.hpp"

#include "Acidum/Core/Platform/KeyboardCode.hpp"
#include "Acidum/Core/Platform/Input.hpp"

#include "Sandbox/Core/Base/Consts.hpp"

namespace Sandbox {

CameraController::CameraController(Acidum::Camera& camera)
    : m_camera(camera) {}
    
void CameraController::onUpdate(float deltaTime) {
    glm::vec2 currentMousePos = Acidum::Input::GetMousePosition();
    if (m_firstMouse) {
        m_lastMousePos = currentMousePos;
        m_firstMouse = false;
    }

    float xoffset = currentMousePos.x - m_lastMousePos.x;
    float yoffset = m_lastMousePos.y - currentMousePos.y;
    m_lastMousePos = currentMousePos;

    float sensitivity = 0.1f;
    m_yaw += xoffset * sensitivity;
    m_pitch += yoffset * sensitivity;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    float cameraSpeed = Consts::CAMERA_SPEED * deltaTime;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::W))
        m_cameraPos += m_camera.getFront() * cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::S))
        m_cameraPos -= m_camera.getFront() * cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::A))
        m_cameraPos -= m_camera.getRight() * cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::D))
        m_cameraPos += m_camera.getRight() * cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::SPACE))
        m_cameraPos.y += cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::L_CTRL))
        m_cameraPos.y -= cameraSpeed;

    m_camera.setRotation(m_pitch, m_yaw);
    m_camera.setPosition(m_cameraPos);
}

} // namespace Sandbox