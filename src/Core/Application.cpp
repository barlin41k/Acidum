#include "Core/Application.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Consts.hpp"
#include "Graphics/GraphicsFactory.hpp"

Application::Application(APIType apiType)
    : m_apiType(apiType) {}

void Application::run() {
    initWindow();
    initGraphicsAPI();
    mainLoop();
}

void Application::initWindow() {
    WindowConfig config {};
    config.width = Consts::WINDOW_WIDTH;
    config.height = Consts::WINDOW_HEIGHT;
    config.title = Consts::WINDOW_TITLE;
    config.apiType = m_apiType;

    m_window = std::make_unique<Window>(config);
}

void Application::initGraphicsAPI() {
    m_graphicsAPI = GraphicsFactory::createAPI(m_apiType, m_window.get());
    m_graphicsAPI->initialize();

    m_triangleMesh = m_graphicsAPI->createMesh(Consts::VERTICES, Consts::INDICES);
}

void Application::mainLoop() {  
    auto startTime = std::chrono::high_resolution_clock::now();

    while (!m_window->shouldClose()) {
        m_window->pollEvents();

        float aspect = 1.0f;
        int width = 0, height = 0;
        m_window->getFramebufferSize(&width, &height);
        if (height > 0) aspect = static_cast<float>(width) / static_cast<float>(height);

        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);

        m_graphicsAPI->setViewMatrix(view);
        m_graphicsAPI->setProjectionMatrix(proj);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        if (m_triangleMesh)
            m_graphicsAPI->drawMesh(m_triangleMesh.get(), modelMatrix);

        m_graphicsAPI->renderFrame();
    }

    m_graphicsAPI->waitIdle();
}