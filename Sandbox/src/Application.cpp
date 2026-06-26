#include "Sandbox/Application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "Acidum/Core/Application.hpp"
#include "Acidum/Core/Platform/Input.hpp"
#include "Acidum/Core/Resources/MaterialSystem.hpp"
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

    Acidum::MaterialSystem::RegisterTemplate(
        Acidum::RenderMode::Opaque,
        "shaders/spirv/shader.vert.spv",
        "shaders/spirv/shader.frag.spv"
    );

    Acidum::MaterialSystem::RegisterTemplate(
        Acidum::RenderMode::Transparent,
        "shaders/spirv/shader.vert.spv",
        "shaders/spirv/shader.frag.spv",
        true, false
    );

    auto ak74Model = Acidum::ResourceManager::loadModel("models/ak74/ak74_acidum.glb");

    Acidum::Entity ak74;
    ak74.model = ak74Model;
    ak74.position = glm::vec3(0.0f, 0.0f, 1.0f);
    ak74.scale = glm::vec3(0.35f);
    ak74.initPose();
    m_entities.push_back(ak74);

    m_akMagIndex = ak74Model->findNodeIndex("magazine");

    GetGraphicsAPI()->endUploadAndWait();
}

void Application::OnUpdate(float deltaTime) {
    m_totalTime += deltaTime;

    updateWindowTitle(deltaTime);
    m_cameraController.onUpdate(deltaTime);
    updateViewProjMatrices();

    glm::vec3 movingSun = glm::vec3(
        std::sin(m_totalTime),
        std::cos(m_totalTime),
        0.0f
    );
    GetGraphicsAPI()->setLightDirection(movingSun);

    if (m_akMagIndex >= 0 && !m_entities.empty()) {
        m_entities[0].nodeOverrides[m_akMagIndex] = glm::rotate(glm::mat4(1.0f), m_totalTime * 3.0f, glm::vec3(1, 0, 0));
    }
}

void Application::OnRender() {
        for (auto& entity : m_entities) {
            if (!entity.model) continue;
            for (size_t i = 0; i < entity.model->nodes.size(); ++i) {
                const auto& node = entity.model->nodes[i];
                glm::mat4 finalMatrix = entity.getTransformMatrix() * node.localTransform * entity.nodeOverrides[i];
                GetGraphicsAPI()->drawMesh(node.mesh.get(), finalMatrix);
            }
        }
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
    GetGraphicsAPI()->setCameraPosition(m_camera.getPosition());

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