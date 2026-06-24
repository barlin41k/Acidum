#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Graphics/Vulkan/VulkanConfigs.hpp"

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanSurface;
class Window;

class VulkanSwapChain {
public:
    VulkanSwapChain(const VulkanDevice& device, const VulkanSurface& surface, Window* window, const SwapChainConfig& config);
    ~VulkanSwapChain();

    VulkanSwapChain(const VulkanSwapChain&) = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

    void recreate();

    VkSwapchainKHR getSwapChain() const { return m_swapChain; }
    VkFormat getFormat() const { return m_swapChainImageFormat; }
    VkExtent2D getExtent() const { return m_swapChainExtent; }

    const std::vector<VkImage>& getImages() const { return m_swapChainImages; }
    const std::vector<VkImageView>& getImageViews() const { return m_swapChainImageViews; }
private:
    const SwapChainConfig m_config; // because swapchain can be recreated
    const VulkanDevice& m_device;
    const VulkanSurface& m_surface;
    Window* m_window;

    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;

    void createSwapChain(VkSwapchainKHR oldSwapChain=VK_NULL_HANDLE);
    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    
    void cleanupSwapChain(VkSwapchainKHR swapChainToDestroy);
    void cleanupSwapChainDependencies();
};

} // namespace Acidum