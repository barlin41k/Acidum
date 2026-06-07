#pragma once

#include <memory>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Platform/Window.hpp"
#include "Acidum/Graphics/Interfaces/IGraphicsAPI.hpp"

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
    virtual ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();
protected:
    virtual void OnInit() {}
    virtual void OnUpdate(float deltaTime) {} // NOLINT
    virtual void OnRender() {}

    IGraphicsAPI* GetGraphicsAPI() { return m_graphicsAPI.get(); }
    Window* GetWindow() { return m_window.get(); }
private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<IGraphicsAPI> m_graphicsAPI;

    AppConfig m_config;
    APIType m_apiType;

    void initWindow();
    void initGraphicsAPI();
    void mainLoop();
};