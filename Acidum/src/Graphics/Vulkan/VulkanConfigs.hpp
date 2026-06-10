#pragma once

#include <vulkan/vulkan.h>

#include <string>

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

struct SwapChainConfig {
    VkFormat preferredFormat = VK_FORMAT_R8G8B8A8_SRGB;
    VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
};

struct PipelineConfig {
    std::vector<char> vertexShaderBytecode;
    std::vector<char> fragmentShaderBytecode;

    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    float lineWidth = 1.0f;

    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    bool enableDepthTest = true;
    bool enableDepthWrite = true;
    VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
    
    bool enableAlphaBlending = false;
};

struct DeviceConfig {
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkPhysicalDeviceFeatures requiredFeatures{};
    
    bool preferDiscreteGPU = true;
};

} // namespace Acidum