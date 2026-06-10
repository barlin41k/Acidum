#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {

class VulkanDevice; // forward-declaration

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
    VulkanPipeline(const VulkanDevice& device, VkFormat swapChainFormat, VkFormat depthFormat, const PipelineConfig& config);
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
    void createGraphicsPipeline(const PipelineConfig& config);
    void createDescriptorSetLayout();

    VkShaderModule createShaderModule(const std::vector<char>& code);
};

} // namespace Acidum