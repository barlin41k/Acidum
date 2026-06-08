#pragma once

#include <vulkan/vulkan.h>

namespace Acidum {

class Window;
class VulkanInstance;

class VulkanSurface {
public:
    VulkanSurface(VulkanInstance& instance, Window* window);
    ~VulkanSurface();

    VulkanSurface(const VulkanSurface&) = delete;
    VulkanSurface& operator=(const VulkanSurface&) = delete;

    VkSurfaceKHR getSurface() const noexcept { return m_surface; }
private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
};

} // namespace Acidum