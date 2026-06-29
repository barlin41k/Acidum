#pragma once

#include <memory>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/Window.hpp"
#include "Acidum/Graphics/Interfaces/IGraphicsAPI.hpp"

namespace Acidum {

struct AppConfig {
    Version version = { 1, 0, 0 };
    std::string title = Consts::ENGINE_NAME;
    uint32_t width = Consts::ENGINE_DEFAULT_WIDTH;
    uint32_t height = Consts::ENGINE_DEFAULT_HEIGHT;
    APIType apiType = Consts::ENGINE_DEFAULT_API;
};

class Application {
public:
    Application(const AppConfig& config);
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

    inline static Application& Get() { ACIDUM_ASSERT(s_Instance, "Application instance is null!"); return *s_Instance; }
    Window* GetWindow() { return m_window.get(); }
protected:
    virtual void OnInit() {}
    virtual void OnUpdate([[maybe_unused]] float deltaTime) {}
    virtual void OnRender() {}

    IGraphicsAPI* GetGraphicsAPI() { return m_graphicsAPI.get(); }
private:
    static Application* s_Instance;

    std::unique_ptr<Window> m_window;
    std::unique_ptr<IGraphicsAPI> m_graphicsAPI;

    AppConfig m_config;

    void initWindow();
    void initGraphicsAPI();
    void mainLoop();
};

} // namespace Acidum