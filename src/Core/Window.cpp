#include "Core/Window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

Window::Window(const WindowConfig& config)
    : m_config(config) {
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::initWindow() {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW!");

    if (m_config.apiType == APIType::Vulkan) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(
        m_config.width, m_config.height,
        m_config.title.c_str(),
        nullptr, nullptr
    );

    if (!m_window)
        throw std::runtime_error("Failed to create GLFW window!");

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

bool Window::shouldClose() const noexcept {
    return glfwWindowShouldClose(m_window);
}

void Window::waitEvents() const noexcept {
    glfwWaitEvents();
}

void Window::pollEvents() {
    glfwPollEvents();
}