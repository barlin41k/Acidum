#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanBuffer {
public:
    VulkanBuffer(VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~VulkanBuffer();

    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VkBuffer getBuffer() const noexcept { return m_buffer; }
    VkDeviceMemory getMemory() const noexcept { return m_memory; }
    VkDeviceSize getSize() const noexcept { return m_size; }

    void map(void** ppData);
    void unmap();
    void copyTo(void* pData, VkDeviceSize size);

    static void copyBuffer(VulkanDevice& device, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
private:
    VulkanDevice& m_device;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDeviceSize m_size;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};