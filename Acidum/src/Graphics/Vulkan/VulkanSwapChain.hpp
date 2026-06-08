#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <vulkan/vulkan_core.h>

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanSurface;
class Window;

struct SwapChainConfig {
    VkFormat preferredFormat = VK_FORMAT_R8G8B8A8_SRGB;
    VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
};

class VulkanSwapChain {
public:
    VulkanSwapChain(const VulkanDevice& device, const VulkanSurface& surface, Window* window, const SwapChainConfig& config);
    ~VulkanSwapChain();

    VulkanSwapChain(const VulkanSwapChain&) = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

    VkSwapchainKHR getSwapChain() const { return m_swapChain; }
    VkFormat getFormat() const { return m_swapChainImageFormat; }
    VkExtent2D getExtent() const { return m_swapChainExtent; }

    const std::vector<VkImageView>& getImageViews() const { return m_swapChainImageViews; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return m_swapChainFramebuffers; }

    void createFramebuffers(VkRenderPass renderPass, VkImageView depthImageView); 
    
    void recreate();
private:
    const VulkanDevice& m_device;
    const VulkanSurface& m_surface;
    Window* m_window;

    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    const SwapChainConfig m_config; // because swapchain can be recreated

    void createSwapChain();
    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void cleanup();
};

} // namespace Acidum