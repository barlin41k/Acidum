#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "Acidum/Graphics/Material.hpp"
#include "Graphics/Vulkan/VulkanTypes.hpp"
#include "Graphics/Vulkan/VulkanDescriptorAllocator.hpp"

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanBuffer;
class VulkanTexture2D;

class VulkanDescriptorManager {
public:
    VulkanDescriptorManager(const VulkanDevice& device, uint32_t maxFramesInFlight);
    ~VulkanDescriptorManager();

    VulkanDescriptorManager(const VulkanDescriptorManager&) = delete;
    VulkanDescriptorManager& operator=(const VulkanDescriptorManager&) = delete;

    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
    
    VkDescriptorSet getGlobalDescriptorSet(uint32_t currentFrame) const { return m_globalDescriptorSets[currentFrame]; }
    VkDescriptorSet getOrCreateMaterialDescriptor(Material* material);
    VkDescriptorSetLayout getGlobalDescriptorSetLayout() const { return m_globalDescriptorSetLayout; }
    VkDescriptorSetLayout getMaterialDescriptorSetLayout() const { return m_materialDescriptorSetLayout; }
private:
    const VulkanDevice& m_device;

    VulkanDescriptorAllocator m_descriptorAllocator;

    VkDescriptorSetLayout m_globalDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_materialDescriptorSetLayout = VK_NULL_HANDLE;
    
    std::vector<VkDescriptorSet> m_globalDescriptorSets;

    std::unordered_map<Material*, VkDescriptorSet> m_materialCache;

    std::vector<std::unique_ptr<VulkanBuffer>> m_uniformBuffers;

    uint32_t m_maxFramesInFlight;
    
    void createDescriptorSetLayouts();
    void createUniformBuffers();
    void createGlobalDescriptorSets();
    VkDescriptorSet buildMaterialDescriptor(Material* material);
};

} // namespace Acidum