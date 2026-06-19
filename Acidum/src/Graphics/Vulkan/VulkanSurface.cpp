#include "Graphics/Vulkan/VulkanSurface.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/Window.hpp"
#include "Graphics/Vulkan/VulkanInstance.hpp"

namespace Acidum {

VulkanSurface::VulkanSurface(const VulkanInstance& instance, Window* window) 
    : m_instance(instance)
{
    ENGINE_VERIFY(glfwCreateWindowSurface(m_instance.getInstance(), window->getWindow(), nullptr, &m_surface) == VK_SUCCESS, "Failed to create window surface!");
}

VulkanSurface::~VulkanSurface() {
    vkDestroySurfaceKHR(m_instance.getInstance(), m_surface, nullptr);
}

} // namespace Acidum