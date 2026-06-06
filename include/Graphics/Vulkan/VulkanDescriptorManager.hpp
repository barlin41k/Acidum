#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "Core/Types.hpp"
#include "Graphics/Vulkan/VulkanBuffer.hpp"

class VulkanDevice; // forward-declaration

class VulkanDescriptorManager {
public:
    VulkanDescriptorManager(VulkanDevice& device, uint32_t maxFramesInFlight, VkDescriptorSetLayout layout);
    ~VulkanDescriptorManager();

    VulkanDescriptorManager(const VulkanDescriptorManager&) = delete;
    VulkanDescriptorManager& operator=(const VulkanDescriptorManager&) = delete;

    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
    
    VkDescriptorSet getDescriptorSet(uint32_t currentFrame) const { return m_descriptorSets[currentFrame]; }
private:
    VulkanDevice& m_device;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;

    uint32_t m_maxFramesInFlight;
    
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets(VkDescriptorSetLayout layout);

    std::vector<std::unique_ptr<VulkanBuffer>> m_uniformBuffers;
};