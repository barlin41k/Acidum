#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <sys/types.h>
#include <iostream>
#include <cstdlib>
#include <memory>

#include "Core/Types.hpp"
#include "Core/Consts.hpp"
#include "Core/Window.hpp"
#include "Graphics/Interfaces/IGraphicsAPI.hpp"
#include "Graphics/Interfaces/IGraphicsFactory.hpp"

class Application {
public:
    Application(APIType apiType)
        : m_apiType(apiType) {}

    void run() {
        initWindow();
        initGraphicsAPI();
        mainLoop();
    }
private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<IGraphicsAPI> m_graphicsAPI;
    
    std::unique_ptr<IMesh> m_triangleMesh;

    APIType m_apiType;

    void initWindow() {
        WindowConfig config {};
        config.width = Consts::WINDOW_WIDTH;
        config.height = Consts::WINDOW_HEIGHT;
        config.title = Consts::WINDOW_TITLE;
        config.apiType = m_apiType;

        m_window = std::make_unique<Window>(config);
    }

    void initGraphicsAPI() {
        m_graphicsAPI = IGraphicsFactory::createAPI(m_apiType, m_window.get());
        m_graphicsAPI->initialize();

        m_triangleMesh = m_graphicsAPI->createMesh(Consts::VERTICES, Consts::INDICES);
    }

    void mainLoop() {
        while (!m_window->shouldClose()) {
            m_window->pollEvents();

            if (m_triangleMesh)
                m_graphicsAPI->drawMesh(m_triangleMesh.get());

            m_graphicsAPI->renderFrame();
        }

        m_graphicsAPI->waitIdle();
    }
};

int main() {
    Application app (Consts::DEFAULT_API);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}