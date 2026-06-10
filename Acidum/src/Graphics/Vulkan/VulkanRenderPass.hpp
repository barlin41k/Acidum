#pragma once

#include <vulkan/vulkan.h>

namespace Acidum {

class VulkanDevice; // forward-declaration

class VulkanRenderPass {
public:
    VulkanRenderPass(const VulkanDevice& device, VkFormat colorFormat, VkFormat depthFormat);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass() const { return m_renderPass; }
private:
    const VulkanDevice& m_device;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    void createRenderPass(VkFormat swapChainFormat, VkFormat depthFormat);
};

} // namespace Acidum