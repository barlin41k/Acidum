#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {

class VulkanDevice; // forward-declaration

class VulkanPipeline {
public:
    VulkanPipeline(const VulkanDevice& device, VkFormat swapChainFormat, VkFormat depthFormat);
    ~VulkanPipeline();

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    VkPipeline getPipeline() const noexcept { return m_graphicsPipeline; }
    VkPipelineLayout getLayout() const noexcept { return m_pipelineLayout; }
    VkRenderPass getRenderPass() const noexcept { return m_renderPass; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }
private:
    const VulkanDevice& m_device;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

    void createRenderPass(VkFormat swapChainFormat, VkFormat depthFormat);
    void createGraphicsPipeline();
    void createDescriptorSetLayout();

    VkShaderModule createShaderModule(const std::vector<char>& code);
};

} // namespace Acidum