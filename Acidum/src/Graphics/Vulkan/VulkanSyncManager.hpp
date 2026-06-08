#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {
class VulkanDevice; // forward-declaration

class VulkanSyncManager {
public:
    VulkanSyncManager(VulkanDevice& device, uint32_t maxFramesInFlight, uint32_t imageCount);
    ~VulkanSyncManager();

    VulkanSyncManager(const VulkanSyncManager&) = delete;
    VulkanSyncManager& operator=(const VulkanSyncManager&) = delete;

    VkSemaphore getAvailableSemaphore(uint32_t currentFrame) const noexcept { return m_imageAvailableSemaphores[currentFrame]; }
    VkSemaphore getFinishedSemaphore(uint32_t imageIndex) const noexcept { return m_renderFinishedSemaphores[imageIndex]; }

    VkFence getInFlightFence(uint32_t currentFrame) const noexcept { return m_inFlightFences[currentFrame]; }
    VkFence* getInFlightFencePtr(uint32_t currentFrame) noexcept { return &m_inFlightFences[currentFrame]; }
    
    VkFence getImageInFlightFence(uint32_t imageIndex) const noexcept { return m_imagesInFlight[imageIndex]; }
    void setImageInFlightFence(uint32_t imageIndex, VkFence fence) noexcept { m_imagesInFlight[imageIndex] = fence; }
private:
    VulkanDevice& m_device;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
};
} // namespace Acidum