#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanRenderPass;

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

class VulkanPipeline {
public:
    VulkanPipeline(const VulkanDevice& device, const VulkanRenderPass& renderPass, const PipelineConfig& config);
    ~VulkanPipeline();

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    VkPipeline getPipeline() const noexcept { return m_graphicsPipeline; }
    VkPipelineLayout getLayout() const noexcept { return m_pipelineLayout; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }
private:
    const VulkanDevice& m_device;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

    void createGraphicsPipeline(const VulkanRenderPass& renderPass, const PipelineConfig& config);
    void createDescriptorSetLayout();

    VkShaderModule createShaderModule(const std::vector<char>& code);
};

} // namespace Acidum