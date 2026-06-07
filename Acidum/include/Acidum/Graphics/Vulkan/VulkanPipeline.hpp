#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

class VulkanDevice; // forward-declaration

class VulkanPipeline {
public:
    VulkanPipeline(VulkanDevice& device, VkFormat swapChainFormat);
    ~VulkanPipeline();

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    VkPipeline getPipeline() const noexcept { return m_graphicsPipeline; }
    VkPipelineLayout getLayout() const noexcept { return m_pipelineLayout; }
    VkRenderPass getRenderPass() const noexcept { return m_renderPass; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }
private:
    VulkanDevice& m_device;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

    void createRenderPass(VkFormat swapChainFormat);
    void createGraphicsPipeline();
    void createDescriptorSetLayout();

    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
};