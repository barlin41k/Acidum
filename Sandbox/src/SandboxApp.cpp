#include "SandboxApp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Acidum/Core/Platform/KeyboardCode.hpp"
#include "Acidum/Core/Platform/Input.hpp"
#include "Acidum/Core/Application.hpp"

namespace Acidum {
    std::unique_ptr<Acidum::Application> CreateApplication() {
        return std::make_unique<SandboxApp>(Consts::ENGINE_DEFAULT_API);
    }
}

static const std::vector<Acidum::Vertex> VERTICES = {
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
};

static const std::vector<uint32_t> INDICES = {
    0, 1, 2, 2, 3, 0,
    5, 4, 7, 7, 6, 5,
    4, 0, 3, 3, 7, 4,
    1, 5, 6, 6, 2, 1,
    3, 2, 6, 6, 7, 3,
    4, 5, 1, 1, 0, 4
};

SandboxApp::SandboxApp(Acidum::APIType apiType) 
    : Acidum::Application(Acidum::AppConfig {
        {0, 1, 0},
        SandboxConsts::WINDOW_TITLE,
        SandboxConsts::WINDOW_WIDTH, SandboxConsts::WINDOW_HEIGHT,
        apiType
    }),
      m_totalTime(0.0f) {}

void SandboxApp::OnInit() {
    Acidum::Input::SetCursorMode(Acidum::CursorMode::Locked);

    GetGraphicsAPI()->beginUpload();

    m_cubeMesh = GetGraphicsAPI()->createMesh(VERTICES, INDICES);

    GetGraphicsAPI()->endUploadAndWait();
}

void SandboxApp::OnUpdate(float deltaTime) {
    m_totalTime += deltaTime;

    updateCamera(deltaTime);
    updateShaderMatrices();
}

void SandboxApp::OnRender() {
    if (m_cubeMesh) {
        glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), m_totalTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        GetGraphicsAPI()->drawMesh(m_cubeMesh.get(), modelMatrix);
    }
}

void SandboxApp::updateCamera(float deltaTime) {
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

    float cameraSpeed = SandboxConsts::CAMERA_SPEED * deltaTime;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::W))
        m_cameraPos += m_camera.getFront() * cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::S))
        m_cameraPos -= m_camera.getFront() * cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::A))
        m_cameraPos -= m_camera.getRight() * cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::D))
        m_cameraPos += m_camera.getRight() * cameraSpeed;

    m_camera.setRotation(m_pitch, m_yaw);
    m_camera.setPosition(m_cameraPos);
}

void SandboxApp::updateShaderMatrices() {
    float aspect = 1.0f;
    int width = 0, height = 0;
    GetWindow()->getFramebufferSize(&width, &height);
    if (width > 0 && height > 0) {
        aspect = static_cast<float>(width) / static_cast<float>(height);
        m_camera.setAspectRatio(aspect);

        GetGraphicsAPI()->setViewMatrix(m_camera.getViewMatrix());
        GetGraphicsAPI()->setProjectionMatrix(m_camera.getProjectionMatrix());
    }
}