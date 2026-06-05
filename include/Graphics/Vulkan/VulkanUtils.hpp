#pragma once

#include <vulkan/vulkan.h>

#include <array>

namespace VulkanUtils {
    VkVertexInputBindingDescription getBindingDescription();
    std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
}