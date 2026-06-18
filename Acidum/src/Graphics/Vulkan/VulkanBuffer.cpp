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

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    
    if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    ENGINE_VERIFY(
        vmaCreateBuffer(m_device.getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr) == VK_SUCCESS, 
        "Failed to create buffer via VMA!"
    );
}

VulkanBuffer::~VulkanBuffer() {
    unmap();
    if (m_buffer != VK_NULL_HANDLE && m_allocation != VK_NULL_HANDLE)
        vmaDestroyBuffer(m_device.getAllocator(), m_buffer, m_allocation);
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

    // TODO: batching with Transfer Queue
    vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.getGraphicsQueue());

    vkFreeCommandBuffers(device.getLogicalDevice(), commandPool, 1, &commandBuffer);
}

void VulkanBuffer::map() {
    if (!m_mappedData)
        vmaMapMemory(m_device.getAllocator(), m_allocation, &m_mappedData);
}

void VulkanBuffer::unmap() {
    if (m_mappedData) {
        vmaUnmapMemory(m_device.getAllocator(), m_allocation);
        m_mappedData = nullptr;
    }
}

void VulkanBuffer::copyTo(const void* pData, VkDeviceSize size) {
    ENGINE_VERIFY(m_mappedData != nullptr, "Buffer must be mapped before copying!");
    std::memcpy(m_mappedData, pData, static_cast<size_t>(size));
}

} // namespace Acidum