#include "Graphics/Vulkan/VulkanDescriptorManager.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanBuffer.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanTexture2D.hpp"

namespace Acidum {

VulkanDescriptorManager::VulkanDescriptorManager(
    const VulkanDevice& device, uint32_t maxFramesInFlight,
    VkDescriptorSetLayout globalLayout, VkDescriptorSetLayout materialLayout
)
    : m_device(device),
      m_descriptorAllocator(device),
      m_materialLayout(materialLayout),
      m_maxFramesInFlight(maxFramesInFlight)
{
    createUniformBuffers();
    createGlobalDescriptorSets(globalLayout);
}

VulkanDescriptorManager::~VulkanDescriptorManager() {
    m_descriptorAllocator.cleanup();
}

void VulkanDescriptorManager::createUniformBuffers() {
    m_uniformBuffers.resize(m_maxFramesInFlight);

    for (uint32_t i = 0; i < m_maxFramesInFlight; i++) {
        m_uniformBuffers[i] = std::make_unique<VulkanBuffer>(
            m_device,
            sizeof(UniformBufferObject),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        m_uniformBuffers[i]->map();
    }
}

void VulkanDescriptorManager::updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo) {
    ENGINE_VERIFY(currentFrame < m_maxFramesInFlight, "currentFrame({}) is out of bounds! Max frames in flight is {}.", currentFrame, m_maxFramesInFlight);
    m_uniformBuffers[currentFrame]->copyTo(&ubo, sizeof(ubo));
}

void VulkanDescriptorManager::createGlobalDescriptorSets(VkDescriptorSetLayout globalLayout) {
    m_globalDescriptorSets.resize(m_maxFramesInFlight);
    for (size_t i = 0; i < m_maxFramesInFlight; i++) {
        ENGINE_VERIFY(
            m_descriptorAllocator.allocate(globalLayout, &m_globalDescriptorSets[i]), 
            "Failed to allocate global descriptor set!"
        );

        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = m_uniformBuffers[i]->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_globalDescriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_device.getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

VkDescriptorSet VulkanDescriptorManager::buildMaterialDescriptor(Material* material) {
    if (!material || !material->albedoTexture) return VK_NULL_HANDLE;

    VkDescriptorSet matSet;
    ENGINE_VERIFY(
        m_descriptorAllocator.allocate(m_materialLayout, &matSet),
        "Failed to allocate material descriptor set!"
    );

    auto vkTexture = std::static_pointer_cast<VulkanTexture2D>(material->albedoTexture);

    VkDescriptorImageInfo imageInfo {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = vkTexture->getImageView();
    imageInfo.sampler = vkTexture->getSampler();

    VkWriteDescriptorSet descriptorWrite {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = matSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device.getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);

    return matSet;
}

VkDescriptorSet VulkanDescriptorManager::getOrCreateMaterialDescriptor(Material* material) {
    if (!material || !material->albedoTexture) return VK_NULL_HANDLE;

    auto item = m_materialCache.find(material);
    if (item != m_materialCache.end())
        return item->second;

    VkDescriptorSet matSet = buildMaterialDescriptor(material);
    m_materialCache[material] = matSet;

    return matSet;
}

} // namespace Acidum