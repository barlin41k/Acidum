#pragma once

#include <vulkan/vulkan.h>

namespace Acidum {

// forward-declaration
class Window;
class VulkanInstance;

class VulkanSurface {
public:
    VulkanSurface(const VulkanInstance& instance, Window* window);
    ~VulkanSurface();

    VulkanSurface(const VulkanSurface&) = delete;
    VulkanSurface& operator=(const VulkanSurface&) = delete;

    VkSurfaceKHR getSurface() const noexcept { return m_surface; }
private:
    const VulkanInstance& m_instance;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
};

} // namespace Acidum