#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {

// forward-declaration
struct InstanceConfig;

class VulkanInstance {
public:
    VulkanInstance(const InstanceConfig& config);
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance& operator=(const VulkanInstance&) = delete;

    VkInstance getInstance() const noexcept { return m_instance; }
private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    void createInstance(const InstanceConfig& config);
    void setupDebugMessenger(const InstanceConfig& config);

    bool checkValidationLayerSupport(const InstanceConfig& config) const;
    std::vector<const char*> getRequiredExtensions(const InstanceConfig& config) const;
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger) const;

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator) const;
};

} // namespace Acidum