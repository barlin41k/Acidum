#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {
class VulkanDevice; // forward-declaration

class VulkanCommandBufferManager {
public:
    VulkanCommandBufferManager(VulkanDevice& device, uint32_t bufferCount);
    ~VulkanCommandBufferManager();

    VulkanCommandBufferManager(const VulkanCommandBufferManager&) = delete;
    VulkanCommandBufferManager& operator=(const VulkanCommandBufferManager&) = delete;

    VkCommandPool getCommandPool() const noexcept { return m_commandPool; }
    VkCommandBuffer getCommandBuffer(uint32_t index) const { return m_commandBuffers[index]; }
    const std::vector<VkCommandBuffer>& getCommandBuffers() const noexcept { return m_commandBuffers; }
private:
    VulkanDevice& m_device;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
};
} // namespace Acidum