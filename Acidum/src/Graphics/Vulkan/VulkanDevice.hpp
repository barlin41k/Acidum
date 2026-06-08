#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace Acidum {
class VulkanInstance;
class VulkanSurface;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct DeviceConfig {
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkPhysicalDeviceFeatures requiredFeatures{};
    
    bool preferDiscreteGPU = true;
};

class VulkanDevice {
public:
    VulkanDevice(const VulkanInstance& instance, const VulkanSurface& surface, const DeviceConfig& config);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    VkPhysicalDevice getPhysicalDevice() const noexcept { return m_physicalDevice; }
    VkDevice getLogicalDevice() const noexcept { return m_device; }
    VkQueue getGraphicsQueue() const noexcept { return m_graphicsQueue; }
    VkQueue getPresentQueue() const noexcept { return m_presentQueue; }

    QueueFamilyIndices getQueueFamilies() const { return findQueueFamilies(m_physicalDevice); }
    SwapChainSupportDetails getSwapChainSupport() const { return querySwapChainSupport(m_physicalDevice); }
private:
    const VulkanInstance& m_instance;
    const VulkanSurface& m_surface;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

    void pickPhysicalDevice(const DeviceConfig& config);
    void createLogicalDevice(const DeviceConfig& config);
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device, const DeviceConfig& config) const;
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;
    bool isDeviceSuitable(VkPhysicalDevice device, const DeviceConfig& config) const;
};
} // namespace Acidum