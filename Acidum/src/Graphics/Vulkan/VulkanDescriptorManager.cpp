#include "Graphics/Vulkan/VulkanDescriptorManager.hpp"

#include "Graphics/Vulkan/VulkanLogger.hpp"
#include "Graphics/Vulkan/VulkanBuffer.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanTexture2D.hpp"

namespace Acidum {

VulkanDescriptorManager::VulkanDescriptorManager(const VulkanDevice& device, uint32_t maxFramesInFlight)
    : m_device(device),
      m_descriptorAllocator(device),
      m_maxFramesInFlight(maxFramesInFlight)
{
    createDescriptorSetLayouts();
    createUniformBuffers();
    createGlobalDescriptorSets();
}

VulkanDescriptorManager::~VulkanDescriptorManager() {
    VkDevice device = m_device.getLogicalDevice();

    m_descriptorAllocator.cleanup();
    vkDestroyDescriptorSetLayout(device, m_globalDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, m_materialDescriptorSetLayout, nullptr);
}

void VulkanDescriptorManager::createDescriptorSetLayouts() {
    VkDevice device = m_device.getLogicalDevice();

    VkDescriptorSetLayoutBinding uboLayoutBinding {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo globalLayoutInfo {};
    globalLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    globalLayoutInfo.bindingCount = 1;
    globalLayoutInfo.pBindings = &uboLayoutBinding;

    ACIDUM_ASSERT(vkCreateDescriptorSetLayout(
        device, &globalLayoutInfo, nullptr, &m_globalDescriptorSetLayout) == VK_SUCCESS,
        "Failed to create global descriptor set layout!"
    );


    std::array<VkDescriptorSetLayoutBinding, 3> bindings {};

    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].pImmutableSamplers = nullptr;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings[1].binding = 1;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].pImmutableSamplers = nullptr;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings[2].binding = 2;
    bindings[2].descriptorCount = 1;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].pImmutableSamplers = nullptr;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo materialLayoutInfo {};
    materialLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    materialLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    materialLayoutInfo.pBindings = bindings.data();

    ACIDUM_ASSERT(
        vkCreateDescriptorSetLayout(device, &materialLayoutInfo, nullptr, &m_materialDescriptorSetLayout) == VK_SUCCESS,
        "Failed to create material descriptor set layout!"
    );
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
    ACIDUM_ASSERT(currentFrame < m_maxFramesInFlight, "currentFrame({}) is out of bounds! Max frames in flight is {}.", currentFrame, m_maxFramesInFlight);
    m_uniformBuffers[currentFrame]->copyTo(&ubo, sizeof(ubo));
}

void VulkanDescriptorManager::createGlobalDescriptorSets() {
    m_globalDescriptorSets.resize(m_maxFramesInFlight);
    for (size_t i = 0; i < m_maxFramesInFlight; i++) {
        ACIDUM_ASSERT(
            m_descriptorAllocator.allocate(m_globalDescriptorSetLayout, &m_globalDescriptorSets[i]), 
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
    if (!material || !material->albedoTexture || !material->metallicRoughnessTexture || !material->normalTexture) return VK_NULL_HANDLE;

    VkDescriptorSet matSet;
    ACIDUM_ASSERT(
        m_descriptorAllocator.allocate(m_materialDescriptorSetLayout, &matSet),
        "Failed to allocate material descriptor set!"
    );

    auto vkAlbedo = std::static_pointer_cast<VulkanTexture2D>(material->albedoTexture);
    auto vkPBR = std::static_pointer_cast<VulkanTexture2D>(material->metallicRoughnessTexture);
    auto vkNorm = std::static_pointer_cast<VulkanTexture2D>(material->normalTexture);

    VkDescriptorImageInfo albedoInfo {};
    albedoInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    albedoInfo.imageView = vkAlbedo->getImageView();
    albedoInfo.sampler = vkAlbedo->getSampler();

    VkDescriptorImageInfo PBRInfo {};
    PBRInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    PBRInfo.imageView = vkPBR->getImageView();
    PBRInfo.sampler = vkPBR->getSampler();

    VkDescriptorImageInfo normInfo {};
    normInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    normInfo.imageView = vkNorm->getImageView();
    normInfo.sampler = vkNorm->getSampler();

    std::array<VkWriteDescriptorSet, 3> descriptorWrites {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = matSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pImageInfo = &albedoInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = matSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &PBRInfo;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = matSet;
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pImageInfo = &normInfo;

    vkUpdateDescriptorSets(m_device.getLogicalDevice(), 3, descriptorWrites.data(), 0, nullptr);

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