#include "Graphics/Vulkan/VulkanSwapChain.hpp"

#include <vulkan/vk_enum_string_helper.h>

#include <algorithm>
#include <vulkan/vulkan_core.h>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/Window.hpp"
#include "Graphics/Vulkan/VulkanSurface.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {
VulkanSwapChain::VulkanSwapChain(const VulkanDevice& device, const VulkanSurface& surface, Window* window, const SwapChainConfig& config)
    : m_device(device),
      m_surface(surface),
      m_window(window),
      m_config(config)
{
    createSwapChain();
    createImageViews();
}

VulkanSwapChain::~VulkanSwapChain() {
    cleanupSwapChainDependencies();
    cleanupSwapChain(m_swapChain);
}

void VulkanSwapChain::cleanupSwapChainDependencies() {
    for (auto framebuffer : m_swapChainFramebuffers)
        vkDestroyFramebuffer(m_device.getLogicalDevice(), framebuffer, nullptr);
    for (auto imageView : m_swapChainImageViews)
        vkDestroyImageView(m_device.getLogicalDevice(), imageView, nullptr);
    
    m_swapChainFramebuffers.clear();
    m_swapChainImageViews.clear();
}

void VulkanSwapChain::cleanupSwapChain(VkSwapchainKHR swapChainToDestroy) {
    if (m_swapChain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(m_device.getLogicalDevice(), swapChainToDestroy, nullptr);
}

void VulkanSwapChain::recreate() {
    VkSwapchainKHR oldSwapchain = m_swapChain;
    cleanupSwapChainDependencies();
    createSwapChain(oldSwapchain);
    cleanupSwapChain(oldSwapchain);
    createImageViews();
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    ENGINE_VERIFY(!availableFormats.empty(), "No available Swap Surface Formats found!");

    bool isFormatSupported = false;
    bool isColorSpaceSupported = false;

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == m_config.preferredFormat) isFormatSupported = true;
        if (availableFormat.colorSpace == m_config.preferredColorSpace) isColorSpaceSupported = true;

        if (availableFormat.format == m_config.preferredFormat && availableFormat.colorSpace == m_config.preferredColorSpace)
            return availableFormat;
    }
    
    if (!isFormatSupported && !isColorSpaceSupported)
        ENGINE_WARN("Swap Surface Format selection failed: neither preferred format ({}) nor color space ({}) are supported by the surface", 
                    static_cast<int>(m_config.preferredFormat), 
                    static_cast<int>(m_config.preferredColorSpace));
    else if (!isFormatSupported)
        ENGINE_WARN("Swap Surface Format mismatch: preferred format ({}) is not supported (but color space is available)", 
                    static_cast<int>(m_config.preferredFormat));
    else if (!isColorSpaceSupported)
        ENGINE_WARN("Swap Surface Format mismatch: preferred color space ({}) is not supported (but format is available)", 
                    static_cast<int>(m_config.preferredColorSpace));
    else
        ENGINE_WARN("Swap Surface Format mismatch: both preferred format ({}) and color space ({}) are supported individually, but not in this combination",
                    static_cast<int>(m_config.preferredFormat),
                    static_cast<int>(m_config.preferredColorSpace));

    ENGINE_WARN("Fallback to default Swap Surface Format. Chosen format: {}, color space: {}", 
                static_cast<int>(availableFormats[0].format), 
                static_cast<int>(availableFormats[0].colorSpace));

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    ENGINE_VERIFY(!availablePresentModes.empty(), "No available Swap Present Modes found!");

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == m_config.preferredPresentMode)
            return availablePresentMode;
    }

    ENGINE_WARN("Swap present mode mismatch: preferred mode ({}) is not supported. Falling back to VK_PRESENT_MODE_FIFO_KHR.",
        static_cast<int>(m_config.preferredPresentMode));

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    else {
        int width, height;
        m_window->getFramebufferSize(&width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanSwapChain::createSwapChain(VkSwapchainKHR oldSwapChain) {
    SwapChainSupportDetails swapChainSupport = m_device.getSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface.getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapChain;

    QueueFamilyIndices indices = m_device.getQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    ENGINE_VERIFY(vkCreateSwapchainKHR(m_device.getLogicalDevice(), &createInfo, nullptr, &m_swapChain) == VK_SUCCESS, "Failed to create swap chain!");

    std::string swapChainState = oldSwapChain == VK_NULL_HANDLE ? "created" : "recreated";
    ENGINE_DEBUG("Vulkan SwapChain {}: {}x{}, Format {}, Present Mode {}",
        swapChainState, extent.width, extent.height,
        string_VkFormat(surfaceFormat.format),
        string_VkPresentModeKHR(presentMode)
    );
    
    vkGetSwapchainImagesKHR(m_device.getLogicalDevice(), m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device.getLogicalDevice(), m_swapChain, &imageCount, m_swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

void VulkanSwapChain::createImageViews() {
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (size_t i = 0; i < m_swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        ENGINE_VERIFY(vkCreateImageView(m_device.getLogicalDevice(), &createInfo, nullptr, &m_swapChainImageViews[i]) == VK_SUCCESS, "Failed to create image views!");
    }
}

void VulkanSwapChain::createFramebuffers(VkRenderPass renderPass, VkImageView depthImageView) {
    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            m_swapChainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_swapChainExtent.width;
        framebufferInfo.height = m_swapChainExtent.height;
        framebufferInfo.layers = 1;

        ENGINE_VERIFY(vkCreateFramebuffer(m_device.getLogicalDevice(), &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) == VK_SUCCESS, "Failed to create framebuffer!");
    }
}
} // namespace Acidum