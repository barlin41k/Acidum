#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "Graphics/Vulkan/VulkanTypes.hpp"

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanBuffer;
class VulkanTexture2D;

class VulkanDescriptorManager {
public:
    VulkanDescriptorManager(const VulkanDevice& device, uint32_t maxFramesInFlight, VkDescriptorSetLayout layout);
    ~VulkanDescriptorManager();

    VulkanDescriptorManager(const VulkanDescriptorManager&) = delete;
    VulkanDescriptorManager& operator=(const VulkanDescriptorManager&) = delete;

    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
    void bindTexture(uint32_t currentFrame, VulkanTexture2D* texture);
    
    VkDescriptorSet getDescriptorSet(uint32_t currentFrame) const { return m_descriptorSets[currentFrame]; }
private:
    const VulkanDevice& m_device;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;

    uint32_t m_maxFramesInFlight;
    
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets(VkDescriptorSetLayout layout);

    std::vector<std::unique_ptr<VulkanBuffer>> m_uniformBuffers;
};

} // namespace Acidum