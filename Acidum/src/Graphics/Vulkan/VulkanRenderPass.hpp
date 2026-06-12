#pragma once

#include <vulkan/vulkan.h>

namespace Acidum {

// forward-declaration
class VulkanDevice;

class VulkanRenderPass {
public:
    VulkanRenderPass(const VulkanDevice& device, VkFormat colorFormat, VkFormat depthFormat);
    ~VulkanRenderPass();

    VulkanRenderPass(const VulkanRenderPass&) = delete;
    VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
    VulkanRenderPass(VulkanRenderPass&&) = delete;
    VulkanRenderPass& operator=(VulkanRenderPass&&) = delete;

    VkRenderPass getRenderPass() const { return m_renderPass; }
private:
    const VulkanDevice& m_device;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    void createRenderPass(VkFormat swapChainFormat, VkFormat depthFormat);
};

} // namespace Acidum