#include "SandboxApp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Acidum/Core/Application.hpp"

namespace Acidum {
    Application* CreateApplication() {
        return new ::SandboxApp(Consts::ENGINE_DEFAULT_API);
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
    : Acidum::Application(Acidum::AppConfig{
        {0, 1, 0},
        SandboxConsts::WINDOW_TITLE,
        SandboxConsts::WINDOW_WIDTH, SandboxConsts::WINDOW_HEIGHT,
        apiType
    }), m_totalTime(0.0f) {}

void SandboxApp::OnInit() {
    m_triangleMesh = GetGraphicsAPI()->createMesh(VERTICES, INDICES);
}

void SandboxApp::OnUpdate(float deltaTime) {
    static float fpsTimer = 0.0f;
    static int frameCount = 0;

    fpsTimer += deltaTime;
    frameCount++;

    if (fpsTimer >= 1.0f) {
        std::string newTitle = std::string(SandboxConsts::WINDOW_TITLE) + " | FPS: " + std::to_string(frameCount);
        GetWindow()->setTitle(newTitle);
        fpsTimer -= 1.0f;
        frameCount = 0;
    }

    m_totalTime += deltaTime;

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

void SandboxApp::OnRender() {
    if (m_triangleMesh) {
        glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), m_totalTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        GetGraphicsAPI()->drawMesh(m_triangleMesh.get(), modelMatrix);
    }
}