#include "Graphics/Vulkan/VulkanDevice.hpp"

#include <vulkan/vk_enum_string_helper.h>

#include <cstdint>
#include <set>

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanConfigs.hpp"
#include "Graphics/Vulkan/VulkanInstance.hpp"
#include "Graphics/Vulkan/VulkanSurface.hpp"

namespace Acidum {

VulkanDevice::VulkanDevice(const VulkanInstance& instance, const VulkanSurface& surface, const DeviceConfig& config)
    : m_instance(instance),
      m_surface(surface)
{
    pickPhysicalDevice(config);
    createLogicalDevice(config);
    createVmaAllocator();
}

VulkanDevice::~VulkanDevice() {
    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device, nullptr);
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) const {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface.getSurface(), &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;

        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
                indices.transferFamily = i;
            else if (!indices.transferFamily.has_value())
                indices.transferFamily = i;
        }

        i++;
    }

    return indices;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device, const DeviceConfig& config) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(config.deviceExtensions.begin(), config.deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device) const {
    VkSurfaceKHR surface = m_surface.getSurface();
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device, const DeviceConfig& config) const {
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device, config);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    const VkBool32* requested = reinterpret_cast<const VkBool32*>(&config.requiredFeatures);
    const VkBool32* supported = reinterpret_cast<const VkBool32*>(&supportedFeatures);

    constexpr size_t featureCount = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);

    for (size_t i = 0; i < featureCount; i++)
        if (requested[i] == VK_TRUE && supported[i] == VK_FALSE)
            return false;

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void VulkanDevice::pickPhysicalDevice(const DeviceConfig& config) {
    VkInstance instance = m_instance.getInstance();

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    ACIDUM_ASSERT(deviceCount > 0, "Failed to find GPUs with Vulkan support!");
    ACIDUM_INFO("Found {} GPUs with Vulkan support", deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    uint64_t highestScore = 0;
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties bestDeviceProperties {};

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        uint32_t version = deviceProperties.apiVersion;
        ACIDUM_DEBUG(
            "Found GPU: {} (Type: {}, Vulkan API: {}.{}.{})",
            deviceProperties.deviceName, static_cast<int>(deviceProperties.deviceType),
            VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version)
        );

        if (!isDeviceSuitable(device, config)) {
            ACIDUM_WARN("{} is not suitable for requirements", deviceProperties.deviceName);
            continue;
        }

        uint64_t score = 0;

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

        if (config.preferDiscreteGPU && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1'000'000;

        VkDeviceSize vramSize = 0;
        for (uint32_t i = 0; i < memProperties.memoryHeapCount; i++) {
            if (memProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                vramSize = std::max(vramSize, memProperties.memoryHeaps[i].size);
        }
        score += static_cast<uint64_t>(vramSize / (1024 * 1024));

        if (score > highestScore) {
            highestScore = score;
            bestDevice = device;
            bestDeviceProperties = deviceProperties;
        }
    }
    
    ACIDUM_ASSERT(bestDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
    m_physicalDevice = bestDevice;

    ACIDUM_INFO("Selected GPU: {} (Score: {})", bestDeviceProperties.deviceName, highestScore);
}

void VulkanDevice::createLogicalDevice(const DeviceConfig& config) {
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value(),
        indices.transferFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceVulkan13Features versionFeatures13 {};
    versionFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    versionFeatures13.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &versionFeatures13;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &config.requiredFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(config.deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = config.deviceExtensions.data();

    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    ACIDUM_ASSERT(
        vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) == VK_SUCCESS,
        "Failed to create logical device!"
    );

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    vkGetDeviceQueue(m_device, indices.transferFamily.value(), 0, &m_transferQueue);
}

void VulkanDevice::createVmaAllocator() {
    VmaAllocatorCreateInfo allocatorInfo {};
    allocatorInfo.physicalDevice = m_physicalDevice;
    allocatorInfo.device = m_device;
    allocatorInfo.instance = m_instance.getInstance();
    allocatorInfo.vulkanApiVersion = m_instance.getApiVersion();

    ACIDUM_ASSERT(
        vmaCreateAllocator(&allocatorInfo, &m_allocator) == VK_SUCCESS,
        "Failed to create VMA allocator!"
    );
}

uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    ACIDUM_ASSERT(false, "Failed to find suitable memory type!");
    
    return 0;
}

VkFormat VulkanDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
    for (VkFormat format : candidates) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &properties);
        
        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
            return format;
    }

    ACIDUM_ASSERT(false, "Failed to find supported format!");
}

VkFormat VulkanDevice::findDepthFormat() const {
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

} // namespace Acidum