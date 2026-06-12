#include "Graphics/Vulkan/VulkanBuffer.hpp"

#include <cstring>

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {

VulkanBuffer::VulkanBuffer(const VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : m_device(device), m_size(size) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    ENGINE_VERIFY(vkCreateBuffer(m_device.getLogicalDevice(), &bufferInfo, nullptr, &m_buffer) == VK_SUCCESS, "Failed to create buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device.getLogicalDevice(), m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memRequirements.memoryTypeBits, properties);

    ENGINE_VERIFY(vkAllocateMemory(m_device.getLogicalDevice(), &allocInfo, nullptr, &m_memory) == VK_SUCCESS, "Failed to allocate buffer memory!");

    vkBindBufferMemory(m_device.getLogicalDevice(), m_buffer, m_memory, 0);
}

VulkanBuffer::~VulkanBuffer() {
    unmap();
    if (m_buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(m_device.getLogicalDevice(), m_buffer, nullptr);
    if (m_memory != VK_NULL_HANDLE)
        vkFreeMemory(m_device.getLogicalDevice(), m_memory, nullptr);
}

void VulkanBuffer::copyBuffer(const VulkanDevice& device, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device.getLogicalDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.getGraphicsQueue());

    vkFreeCommandBuffers(device.getLogicalDevice(), commandPool, 1, &commandBuffer);
}

void VulkanBuffer::map() {
    if (!m_mappedData)
        vkMapMemory(m_device.getLogicalDevice(), m_memory, 0, m_size, 0, &m_mappedData);
}

void VulkanBuffer::unmap() {
    if (m_mappedData) {
        vkUnmapMemory(m_device.getLogicalDevice(), m_memory);
        m_mappedData = nullptr;
    }
}

void VulkanBuffer::copyTo(const void* pData, VkDeviceSize size) {
    ENGINE_VERIFY(m_mappedData != nullptr, "Buffer must be mapped before copying!");
    std::memcpy(m_mappedData, pData, static_cast<size_t>(size));
}

} // namespace Acidum