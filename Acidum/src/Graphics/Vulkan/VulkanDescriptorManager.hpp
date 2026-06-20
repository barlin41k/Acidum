#pragma once

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <memory>
#include <vector>

#include "Acidum/Graphics/Material.hpp"
#include "Graphics/Vulkan/VulkanTypes.hpp"

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanBuffer;
class VulkanTexture2D;

class VulkanDescriptorManager {
public:
    VulkanDescriptorManager(const VulkanDevice& device, uint32_t maxFramesInFlight, VkDescriptorSetLayout globalLayout, VkDescriptorSetLayout materialLayout);
    ~VulkanDescriptorManager();

    VulkanDescriptorManager(const VulkanDescriptorManager&) = delete;
    VulkanDescriptorManager& operator=(const VulkanDescriptorManager&) = delete;

    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
    
    VkDescriptorSet getGlobalDescriptorSet(uint32_t currentFrame) const { return m_globalDescriptorSets[currentFrame]; }
    VkDescriptorSet getMaterialDescriptorSet(Material* material, uint32_t currentFrame);
private:
    const VulkanDevice& m_device;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_globalDescriptorSets;

    VkDescriptorSetLayout m_materialLayout;
    std::unordered_map<Material*, std::vector<VkDescriptorSet>> m_materialSetsCache;

    uint32_t m_maxFramesInFlight;
    
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets(VkDescriptorSetLayout globalLayout);

    std::vector<std::unique_ptr<VulkanBuffer>> m_uniformBuffers;
};

} // namespace Acidum