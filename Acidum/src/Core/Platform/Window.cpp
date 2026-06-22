#include "Acidum/Core/Platform/Window.hpp"

#include <GLFW/glfw3.h>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/Input.hpp"

namespace Acidum {

Window::Window(const WindowConfig& config)
    : m_config(config)
{
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(m_window);
}

void Window::initWindow() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(
        static_cast<int>(m_config.width), static_cast<int>(m_config.height),
        m_config.title.c_str(),
        nullptr, nullptr
    );

    ENGINE_VERIFY(m_window, "Failed to create GLFW window!");
    
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, internalResizeCallback);
    glfwSetWindowFocusCallback(m_window, windowFocusCallback);
}

void Window::getFramebufferSize(int* width, int* height) const noexcept {
    glfwGetFramebufferSize(m_window, width, height);
}

void Window::setTitle(const std::string& title) {
    glfwSetWindowTitle(m_window, title.c_str());
}

void Window::internalResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (windowInstance && windowInstance->m_resizeCallback)
        windowInstance->m_resizeCallback(width, height);
}

void Window::windowFocusCallback([[maybe_unused]] GLFWwindow* glfwWindow, int focused) {
    if (focused)
        Input::SetCursorMode(CursorMode::Locked);
    else
        Input::SetCursorMode(CursorMode::Normal);
}

bool Window::shouldClose() const noexcept {
    return glfwWindowShouldClose(m_window);
}

void Window::waitEvents() const noexcept {
    glfwWaitEvents();
}

void Window::pollEvents() {
    glfwPollEvents();
}

} // namespace Acidum