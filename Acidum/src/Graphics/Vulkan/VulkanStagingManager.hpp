#pragma once

#include "Graphics/Vulkan/VulkanBuffer.hpp"
#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>
#include <memory>

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanBuffer;

class VulkanStagingManager {
public:
    VulkanStagingManager(const VulkanDevice& device);
    ~VulkanStagingManager();

    void begin();
    void submit();
    void waitForUpload();

    void stageCopy(std::unique_ptr<VulkanBuffer> stagingBuffer, VkBuffer dst, VkDeviceSize size);
    void stageCopy(std::unique_ptr<VulkanBuffer> stagingBuffer, VkImage dstImage, uint32_t width, uint32_t height);
private:
    const VulkanDevice& m_device;

    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkFence m_uploadFence = VK_NULL_HANDLE;
    std::vector<std::unique_ptr<VulkanBuffer>> m_stagingBuffers;

    const uint32_t m_commandBuffersCount = 1;

    void createCommandPool();
    void allocateCommandBuffers(uint32_t bufferCount);
};

} // namespace Acidum