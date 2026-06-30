#include "Graphics/Vulkan/VulkanBuffer.hpp"

#include <cstring>

#include "Graphics/Vulkan/VulkanLogger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {

VulkanBuffer::VulkanBuffer(const VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : m_device(device),
      m_size(size)
{    
    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    
    if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    ACIDUM_ASSERT(
        vmaCreateBuffer(m_device.getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr) == VK_SUCCESS, 
        "Failed to create buffer via VMA!"
    );
}

VulkanBuffer::~VulkanBuffer() {
    unmap();
    vmaDestroyBuffer(m_device.getAllocator(), m_buffer, m_allocation);
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
    ACIDUM_ASSERT(m_mappedData != nullptr, "Buffer must be mapped before copying!");
    std::memcpy(m_mappedData, pData, static_cast<size_t>(size));
}

} // namespace Acidum