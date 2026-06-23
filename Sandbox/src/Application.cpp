#include "Sandbox/Application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "Acidum/Core/Platform/Input.hpp"
#include "Acidum/Core/Application.hpp"
#include "Acidum/Core/Resources/ResourceManager.hpp"

#include "Sandbox/Core/Base/Consts.hpp"

namespace Acidum {

std::unique_ptr<Acidum::Application> CreateApplication() {
    return std::make_unique<Sandbox::Application>(Consts::ENGINE_DEFAULT_API);
}

} // namespace Acidum


namespace Sandbox {

Application::Application(Acidum::APIType apiType) 
    : Acidum::Application({
        Consts::APPLICATION_VERSION,
        Consts::WINDOW_TITLE,
        Consts::WINDOW_WIDTH, Consts::WINDOW_HEIGHT,
        apiType
    }),
      m_camera(
        Consts::CAMERA_FOV,
        static_cast<float>(Consts::WINDOW_WIDTH) / static_cast<float>(Consts::WINDOW_HEIGHT),
        Consts::CAMERA_NEAR_CLIP, Consts::CAMERA_FAR_CLIP
    ),
      m_cameraController(m_camera) {}

void Application::OnInit() {
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

void Application::OnUpdate(float deltaTime) {
    m_totalTime += deltaTime;

    updateWindowTitle(deltaTime);
    m_cameraController.onUpdate(deltaTime);
    updateViewProjMatrices();

    if (!m_entities.empty())
        m_entities[0].rotation.y = m_totalTime * glm::radians(90.0f);

    glm::vec3 movingSun = glm::vec3(
        std::sin(m_totalTime),
        1.0f,
        std::cos(m_totalTime)
    );
    GetGraphicsAPI()->setLightDirection(movingSun);
}

void Application::OnRender() {
    for (auto& entity : m_entities)
        GetGraphicsAPI()->drawMesh(entity.mesh.get(), entity.getTransformMatrix());
}

void Application::updateWindowTitle(float deltaTime) {
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

void Application::updateViewProjMatrices() {
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

} // namespace Sandbox