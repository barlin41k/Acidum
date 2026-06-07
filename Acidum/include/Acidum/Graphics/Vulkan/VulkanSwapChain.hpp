#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {
// forward-declaration
class VulkanDevice;
class Window;

class VulkanSwapChain {
public:
    VulkanSwapChain(VulkanDevice& device, VkSurfaceKHR surface, Window* window);
    ~VulkanSwapChain();

    VulkanSwapChain(const VulkanSwapChain&) = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

    VkSwapchainKHR getSwapChain() const { return m_swapChain; }
    VkFormat getFormat() const { return m_swapChainImageFormat; }
    VkExtent2D getExtent() const { return m_swapChainExtent; }

    const std::vector<VkImageView>& getImageViews() const { return m_swapChainImageViews; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return m_swapChainFramebuffers; }

    void createFramebuffers(VkRenderPass renderPass); 
    
    void recreate();
private:
    VulkanDevice& m_device;
    VkSurfaceKHR m_surface;
    Window* m_window;

    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    void createSwapChain();
    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void cleanup();
};
} // namespace Acidum