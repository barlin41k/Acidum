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
      m_materialLayout(materialLayout),
      m_maxFramesInFlight(maxFramesInFlight)
{
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets(globalLayout);
}

VulkanDescriptorManager::~VulkanDescriptorManager() {
    vkDestroyDescriptorPool(m_device.getLogicalDevice(), m_descriptorPool, nullptr);
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

void VulkanDescriptorManager::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(m_maxFramesInFlight * 100); // TODO: DescriptorAllocator
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(m_maxFramesInFlight * 100); // TODO: DescriptorAllocator

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(m_maxFramesInFlight * 100); // TODO: DescriptorAllocator

    ENGINE_VERIFY(vkCreateDescriptorPool(
        m_device.getLogicalDevice(), &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS,
        "Failed to create descriptor pool!"
    );
}

void VulkanDescriptorManager::createDescriptorSets(VkDescriptorSetLayout globalLayout) { // rename
    std::vector<VkDescriptorSetLayout> globalLayouts(m_maxFramesInFlight, globalLayout);
    VkDescriptorSetAllocateInfo globalAllocInfo{};
    globalAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    globalAllocInfo.descriptorPool = m_descriptorPool;
    globalAllocInfo.descriptorSetCount = m_maxFramesInFlight;
    globalAllocInfo.pSetLayouts = globalLayouts.data();

    m_globalDescriptorSets.resize(m_maxFramesInFlight);
    ENGINE_VERIFY(
        vkAllocateDescriptorSets(m_device.getLogicalDevice(), &globalAllocInfo, m_globalDescriptorSets.data()) == VK_SUCCESS, 
        "Failed to allocate global sets!"
    );

    for (size_t i = 0; i < m_maxFramesInFlight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uniformBuffers[i]->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
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

VkDescriptorSet VulkanDescriptorManager::getMaterialDescriptorSet(Material* material, uint32_t currentFrame) {
    if (!material) return VK_NULL_HANDLE;

    if (m_materialSetsCache.find(material) != m_materialSetsCache.end())
        return m_materialSetsCache[material][currentFrame];

    std::vector<VkDescriptorSetLayout> layouts(m_maxFramesInFlight, m_materialLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = m_maxFramesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> newSets(m_maxFramesInFlight);
    vkAllocateDescriptorSets(m_device.getLogicalDevice(), &allocInfo, newSets.data());

    if (material->albedoTexture) {
        auto vkTexture = std::static_pointer_cast<VulkanTexture2D>(material->albedoTexture);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = vkTexture->getImageView();
        imageInfo.sampler = vkTexture->getSampler();

        std::vector<VkWriteDescriptorSet> descriptorWrites(m_maxFramesInFlight);
        for (size_t i = 0; i < m_maxFramesInFlight; i++) {
            descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i].dstSet = newSets[i];
            descriptorWrites[i].dstBinding = 0;
            descriptorWrites[i].dstArrayElement = 0;
            descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[i].descriptorCount = 1;
            descriptorWrites[i].pImageInfo = &imageInfo;
        }
        vkUpdateDescriptorSets(m_device.getLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    m_materialSetsCache[material] = newSets;
    return newSets[currentFrame];
}

} // namespace Acidum