#include "Sandbox/Application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "Acidum/Core/Application.hpp"
#include "Acidum/Core/Platform/Input.hpp"
#include "Acidum/Graphics/MaterialSystem.hpp"
#include "Acidum/Core/Resources/ResourceManager.hpp"

#include "Sandbox/Core/Base/Consts.hpp"

namespace Acidum {

std::unique_ptr<Acidum::Application> CreateApplication() {
    return std::make_unique<Sandbox::Application>(Consts::RENDERER::DEFAULT_API_TYPE);
}

} // namespace Acidum


namespace Sandbox {

Application::Application(Acidum::APIType apiType) 
    : Acidum::Application(Acidum::AppConfig {
        .version = Consts::APPLICATION::VERSION,
        .title = Consts::WINDOW::TITLE,
        .width = Consts::WINDOW::WIDTH, 
        .height = Consts::WINDOW::HEIGHT,
        .apiType = apiType,
        .config = {
            .enableVSync = false,
            .gpuPreference = Acidum::GPUPreference::HighPerformance,
            .supportWireframe = true
        }
    }),
      m_camera(
        Consts::RENDERER::CAMERA::FOV,
        static_cast<float>(Consts::WINDOW::WIDTH) / static_cast<float>(Consts::WINDOW::HEIGHT),
        Consts::RENDERER::CAMERA::NEAR_CLIP, Consts::RENDERER::CAMERA::FAR_CLIP
    ),
      m_cameraController(m_camera) {}

void Application::OnInit() {
    Acidum::Input::SetCursorMode(Acidum::CursorMode::Locked);

    GetGraphicsAPI()->beginUpload();

    Acidum::ResourceManager::getMissingTexture();
    Acidum::ResourceManager::getMissingNormalTexture();

    Acidum::MaterialSystem::RegisterTemplate(
        Acidum::RenderMode::Opaque,
        {
            .vertShaderPath = "shaders/spirv/pbr.vert.spv",
            .fragShaderPath = "shaders/spirv/pbr.frag.spv",
        }
    );

    Acidum::MaterialSystem::RegisterTemplate(
        Acidum::RenderMode::Transparent,
        {
            .vertShaderPath = "shaders/spirv/pbr.vert.spv",
            .fragShaderPath = "shaders/spirv/pbr.frag.spv",
            .enableBlending = true,
            .enableDepthWrite = false
        }
    );

    Acidum::MaterialSystem::RegisterCustomTemplate(
        "Hologram",
        {
            .vertShaderPath = "shaders/spirv/holo.vert.spv",
            .fragShaderPath = "shaders/spirv/holo.frag.spv",
            .enableBlending = true,
            .enableDepthWrite = false
        }
    );

    auto holoMat = Acidum::MaterialSystem::CreateCustomMaterial("Hologram");

    auto ak74Model = Acidum::ResourceManager::loadModel("models/ak74/ak74_acidum.glb");

    GetGraphicsAPI()->endUploadAndWait();

    ak74Model->setMaterialForNode("magazine", holoMat);

    Acidum::Entity ak74;
    ak74.model = ak74Model;
    ak74.position = glm::vec3(0.0f);
    ak74.scale = glm::vec3(0.35f);
    ak74.initPose();
    
    m_entities.push_back(ak74);
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