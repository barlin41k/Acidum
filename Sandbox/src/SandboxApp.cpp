#include "SandboxApp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "Acidum/Core/Platform/KeyboardCode.hpp"
#include "Acidum/Core/Platform/Input.hpp"
#include "Acidum/Core/Application.hpp"
#include "Acidum/Core/Resources/ResourceManager.hpp"

namespace Acidum {
    std::unique_ptr<Acidum::Application> CreateApplication() {
        return std::make_unique<SandboxApp>(Consts::ENGINE_DEFAULT_API);
    }
}

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

    auto mat1 = std::make_shared<Acidum::Material>("shaders/spirv/shader.vert.spv", "shaders/spirv/shader.frag.spv");
    mat1->albedoTexture = Acidum::ResourceManager::loadTexture("textures/Garlic_u1_v1.jpg");
    
    auto mesh1 = Acidum::ResourceManager::loadMesh("models/Garlic.obj");
    mesh1->setMaterial(mat1);

    Acidum::Entity garlic1;
    garlic1.mesh = mesh1;
    garlic1.position = glm::vec3(0.0f);
    garlic1.scale = glm::vec3(0.01f);
    m_entities.push_back(garlic1);

    Acidum::Entity garlic2;
    garlic2.mesh = mesh1;
    garlic2.position = glm::vec3(0.0f, 0.2f, 0.0f);
    garlic2.scale = glm::vec3(0.03f);
    m_entities.push_back(garlic2);

    GetGraphicsAPI()->endUploadAndWait();
}

void SandboxApp::OnUpdate(float deltaTime) {
    m_totalTime += deltaTime;

    updateWindowTitle(deltaTime);
    updateCamera(deltaTime);
    updateViewProjMatrices();

    if (!m_entities.empty())
        m_entities[0].rotation.y = m_totalTime * glm::radians(90.0f);
}

void SandboxApp::OnRender() {
    for (auto& entity : m_entities)
        GetGraphicsAPI()->drawMesh(entity.mesh.get(), entity.getTransformMatrix());
}

void SandboxApp::updateWindowTitle(float deltaTime) {
    m_fpsTimer += deltaTime;
    m_frameCount++;

    if (m_fpsTimer >= 1.0f) {
        float fps = m_frameCount / m_fpsTimer; 
        
        std::string newTitle = "FPS: " + std::to_string(static_cast<int>(fps));
        GetWindow()->setTitle(newTitle);

        m_fpsTimer = 0.0f;
        m_frameCount = 0;
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
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::SPACE))
        m_cameraPos.y += cameraSpeed;
    if (Acidum::Input::IsKeyPressed(Acidum::KeyCode::L_CTRL))
        m_cameraPos.y -= cameraSpeed;

    m_camera.setRotation(m_pitch, m_yaw);
    m_camera.setPosition(m_cameraPos);
}

void SandboxApp::updateViewProjMatrices() {
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