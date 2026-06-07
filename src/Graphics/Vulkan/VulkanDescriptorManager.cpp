#include "Graphics/Vulkan/VulkanDescriptorManager.hpp"

#include "Core/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

VulkanDescriptorManager::VulkanDescriptorManager(VulkanDevice& device, uint32_t maxFramesInFlight, VkDescriptorSetLayout layout)
    : m_device(device), m_maxFramesInFlight(maxFramesInFlight) {
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets(layout);
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
    }
}

void VulkanDescriptorManager::updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo) {
    ENGINE_VERIFY(currentFrame < m_maxFramesInFlight, "currentFrame({}) is out of bounds! Max frames in flight is {}.", currentFrame, m_maxFramesInFlight);
    m_uniformBuffers[currentFrame]->copyTo(const_cast<UniformBufferObject*>(&ubo), sizeof(ubo));
}

void VulkanDescriptorManager::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(m_maxFramesInFlight);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(m_maxFramesInFlight);

    ENGINE_VERIFY(vkCreateDescriptorPool(m_device.getLogicalDevice(), &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS, "Failed to create descriptor pool!");
    ENGINE_DEBUG("Vulkan Descriptor Pool created!");
}

void VulkanDescriptorManager::createDescriptorSets(VkDescriptorSetLayout layout) {
    std::vector<VkDescriptorSetLayout> layouts(m_maxFramesInFlight, layout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_maxFramesInFlight);
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(m_maxFramesInFlight);
    ENGINE_VERIFY(vkAllocateDescriptorSets(m_device.getLogicalDevice(), &allocInfo, m_descriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");
    ENGINE_DEBUG("Vulkan Descriptor Sets allocated!");

    for (size_t i = 0; i < m_maxFramesInFlight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uniformBuffers[i]->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(m_device.getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}