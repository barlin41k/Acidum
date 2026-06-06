#pragma once

#include <memory>

#include "Core/Types.hpp"
#include "Core/Window.hpp"
#include "Graphics/Interfaces/IMesh.hpp"
#include "Graphics/Interfaces/IGraphicsAPI.hpp"

class Application {
public:
    Application(APIType apiType);
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();
private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<IGraphicsAPI> m_graphicsAPI;

    std::unique_ptr<IMesh> m_triangleMesh;

    APIType m_apiType;

    void initWindow();
    void initGraphicsAPI();
    void mainLoop();
};