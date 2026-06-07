#include "Acidum/Core/Platform/Window.hpp"

#include <GLFW/glfw3.h>

#include "Acidum/Core/Base/Logger.hpp"

Window::Window(const WindowConfig& config)
    : m_config(config) {
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::initWindow() {
    glfwSetErrorCallback(GLFWErrorCallback);

    ENGINE_VERIFY(glfwInit(), "Failed to initialize GLFW!");
    ENGINE_DEBUG("GLFW initialized!");

    if (m_config.apiType == APIType::Vulkan)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(
        static_cast<int>(m_config.width), static_cast<int>(m_config.height),
        m_config.title.c_str(),
        nullptr, nullptr
    );

    ENGINE_VERIFY(m_window, "Failed to create GLFW window!");
    ENGINE_INFO("GLFW Window created!");
    
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, internalResizeCallback);
}

void Window::getFramebufferSize(int* width, int* height) const noexcept {
    glfwGetFramebufferSize(m_window, width, height);
}

void Window::internalResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    auto windowInstance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (windowInstance && windowInstance->m_resizeCallback)
        windowInstance->m_resizeCallback(width, height);
}

void Window::GLFWErrorCallback(int error, const char* description) {
    ENGINE_ERROR("GLFW Error ({}): {}", error, description);
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