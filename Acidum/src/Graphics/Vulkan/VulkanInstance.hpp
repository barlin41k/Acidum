#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Acidum/Core/Base/Consts.hpp"

namespace Acidum {

struct InstanceConfig {
    std::string appName = Consts::ENGINE_NAME;
    uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
    uint32_t apiVersion = VK_API_VERSION_1_2;

    std::vector<const char*> windowExtensions;
    std::vector<const char*> additionalExtensions;
    std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#ifdef NDEBUG
    bool enableValidationLayers = false;
#else
    bool enableValidationLayers = true;
#endif
};

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

    bool m_enableValidationLayers;
    std::vector<const char*> m_validationLayers;

    void createInstance(const InstanceConfig& config);
    void setupDebugMessenger();

    bool checkValidationLayerSupport() const;
    std::vector<const char*> getRequiredExtensions(const std::vector<const char*>& windowExtensions) const;
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