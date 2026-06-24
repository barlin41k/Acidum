#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {

// forward-declaration
class VulkanDevice;
struct PipelineConfig;

class VulkanPipeline {
public:
    VulkanPipeline(const VulkanDevice& device, const PipelineConfig& config, const std::vector<VkDescriptorSetLayout>& layouts);
    ~VulkanPipeline();

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    VkPipeline getPipeline() const noexcept { return m_graphicsPipeline; }
    VkPipelineLayout getLayout() const noexcept { return m_pipelineLayout; }
private:
    const VulkanDevice& m_device;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createGraphicsPipeline(const PipelineConfig& config, const std::vector<VkDescriptorSetLayout>& layouts);
};

} // namespace Acidum