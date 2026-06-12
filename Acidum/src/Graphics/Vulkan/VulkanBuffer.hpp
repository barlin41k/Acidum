#pragma once

#include <vulkan/vulkan.h>

namespace Acidum {

// forward-declaration
class VulkanDevice;

class VulkanBuffer {
public:
    VulkanBuffer(const VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~VulkanBuffer();

    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VkBuffer getBuffer() const noexcept { return m_buffer; }
    VkDeviceMemory getMemory() const noexcept { return m_memory; }
    VkDeviceSize getSize() const noexcept { return m_size; }

    void map();
    void unmap();
    void copyTo(const void* pData, VkDeviceSize size);

    static void copyBuffer(const VulkanDevice& device, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
private:
    const VulkanDevice& m_device;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDeviceSize m_size;
    void* m_mappedData = nullptr;
};

} // namespace Acidum