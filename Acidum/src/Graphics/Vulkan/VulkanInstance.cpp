#include "Graphics/Vulkan/VulkanInstance.hpp"

#include <vulkan/vulkan.h>

#include <cstring>

#include "Acidum/Core/Base/Consts.hpp"
#include "Graphics/Vulkan/VulkanLogger.hpp"
#include "Graphics/Vulkan/VulkanConfigs.hpp"

namespace Acidum {

VulkanInstance::VulkanInstance(const InstanceConfig& config)
    : m_apiVersion(config.apiVersion)
{
    createInstance(config);
    setupDebugMessenger(config);
}

VulkanInstance::~VulkanInstance() {
    if (m_instance != VK_NULL_HANDLE && m_debugMessenger != VK_NULL_HANDLE)
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    if (m_instance != VK_NULL_HANDLE)
        vkDestroyInstance(m_instance, nullptr);
}

void VulkanInstance::createInstance(const InstanceConfig& config) {
    ACIDUM_ASSERT(
        !config.enableValidationLayers || checkValidationLayerSupport(config),
        "Validation layers requested, but not available!"
    );

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = config.appName.c_str();
    appInfo.applicationVersion = config.appVersion;
    appInfo.pEngineName = Consts::ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_VERSION(
        Consts::ENGINE_VERSION.major, 
        Consts::ENGINE_VERSION.minor,
        Consts::ENGINE_VERSION.patch
    );
    appInfo.apiVersion = config.apiVersion;

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions(config);
    for (const char* ext : config.additionalExtensions)
        extensions.push_back(ext);

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    if (config.enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(config.validationLayers.size());
        createInfo.ppEnabledLayerNames = config.validationLayers.data();
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    ACIDUM_ASSERT(
        vkCreateInstance(&createInfo, nullptr, &m_instance) == VK_SUCCESS,
        "Failed to create instance!"
    );
}

void VulkanInstance::setupDebugMessenger(const InstanceConfig& config) {
    if (!config.enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    ACIDUM_ASSERT(
        CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) == VK_SUCCESS,
        "Failed to set up debug messenger!"
    );
}

bool VulkanInstance::checkValidationLayerSupport(const InstanceConfig& config) const {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : config.validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

std::vector<const char*> VulkanInstance::getRequiredExtensions(const InstanceConfig& config) const {
    std::vector<const char*> extensions(config.windowExtensions);

    if (config.enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

void VulkanInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    //                       | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT; // a lot of spam

    createInfo.pfnUserCallback = debugCallback;
}

VkBool32 VulkanInstance::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /*pUserData*/) {

    std::string type;
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        type = "[GENERAL] Vulkan";
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        type = "[VALIDATION] Vulkan";
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        type = "[PERFORMANCE] Vulkan";
    if (type.empty())
        type = "[UNKNOWN] Vulkan";

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        VK_ERROR("{}: {}", type, pCallbackData->pMessage);
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        VK_WARN("{}: {}", type, pCallbackData->pMessage);
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        VK_DEBUG("{}: {}", type, pCallbackData->pMessage);
    else
        VK_TRACE("{}: {}", type, pCallbackData->pMessage); 

    return VK_FALSE;
}

VkResult VulkanInstance::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) const {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanInstance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) const {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) func(instance, debugMessenger, pAllocator);
}

} // namespace Acidum