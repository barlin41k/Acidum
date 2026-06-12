#pragma once

#include <functional>
#include <cstdint>
#include <string>

#include "Acidum/Core/Base/Types.hpp"

struct GLFWwindow; // forward-declaration

namespace Acidum {

struct WindowConfig {
    uint32_t width;
    uint32_t height;
    std::string title;
    APIType apiType;
    Version version;
};

class Window {
public:
    Window(const WindowConfig& config);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const noexcept;
    void waitEvents() const noexcept;
    void pollEvents();

    GLFWwindow* getWindow() const noexcept { return m_window; }
    const std::string& getTitle() const noexcept { return m_config.title; }
    const Version& getVersion() const noexcept { return m_config.version; }
    void getFramebufferSize(int* width, int* height) const noexcept;

    void setTitle(const std::string& title);
    void setResizeCallback(std::function<void(int, int)> callback) { m_resizeCallback = callback; }
private:
    GLFWwindow* m_window = nullptr;
    const WindowConfig m_config;
    std::function<void(int, int)> m_resizeCallback = nullptr;

    void initWindow();
    static void internalResizeCallback(GLFWwindow* glfwWindow, int width, int height);
};
} // namespace Acidum