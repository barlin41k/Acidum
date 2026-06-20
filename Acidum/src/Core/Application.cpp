#include "Acidum/Core/Application.hpp"

#include <chrono>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/GLFWContext.hpp"
#include "Acidum/Graphics/GraphicsFactory.hpp"
#include "Acidum/Core/Resources/ResourceManager.hpp"

namespace Acidum {

Application* Application::s_Instance = nullptr;

Application::Application(const AppConfig& config)
    : m_config(config)
{
    ENGINE_VERIFY(!s_Instance, "Application already exists!");
    s_Instance = this;
}

Application::~Application() {
    ResourceManager::shutdown();
    m_graphicsAPI.reset();
    m_window.reset();
    GLFWContext::Shutdown();
}

void Application::run() {
    GLFWContext::Init();
    ResourceManager::initialize();
    initWindow();
    initGraphicsAPI();
    ResourceManager::setGraphicsAPI(m_graphicsAPI.get());
    mainLoop();
}

void Application::initWindow() {
    WindowConfig config {};
    config.width = m_config.width;
    config.height = m_config.height;
    config.title = m_config.title;
    config.apiType = m_config.apiType;
    config.version = m_config.version;

    m_window = std::make_unique<Window>(config);
}

void Application::initGraphicsAPI() {
    m_graphicsAPI = GraphicsFactory::createAPI(m_config.apiType, m_window.get());
    m_graphicsAPI->initialize();
}

void Application::mainLoop() {  
    OnInit();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!m_window->shouldClose()) {
        m_window->pollEvents();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        OnUpdate(deltaTime);
        OnRender();

        m_graphicsAPI->renderFrame();
    }

    m_graphicsAPI->waitIdle();
}

} // namespace Acidum