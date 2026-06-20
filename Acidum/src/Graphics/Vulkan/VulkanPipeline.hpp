#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanRenderPass;
struct PipelineConfig;

class VulkanPipeline {
public:
    VulkanPipeline(const VulkanDevice& device, const VulkanRenderPass& renderPass, const PipelineConfig& config);
    ~VulkanPipeline();

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    VkPipeline getPipeline() const noexcept { return m_graphicsPipeline; }
    VkPipelineLayout getLayout() const noexcept { return m_pipelineLayout; }

    VkDescriptorSetLayout getGlobalDescriptorSetLayout() const { return m_globalDescriptorSetLayout; }
    VkDescriptorSetLayout getMaterialDescriptorSetLayout() const { return m_materialDescriptorSetLayout; }
private:
    const VulkanDevice& m_device;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_globalDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_materialDescriptorSetLayout = VK_NULL_HANDLE;

    void createGraphicsPipeline(const VulkanRenderPass& renderPass, const PipelineConfig& config);
    void createDescriptorSetLayout();

    VkShaderModule createShaderModule(const std::vector<char>& code);
};

} // namespace Acidum