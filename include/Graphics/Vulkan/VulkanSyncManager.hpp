#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class VulkanDevice; // forward-declaration

class VulkanSyncManager {
public:
    VulkanSyncManager(VulkanDevice& device, uint32_t maxFramesInFlight, uint32_t imageCount);
    ~VulkanSyncManager();

    VulkanSyncManager(const VulkanSyncManager&) = delete;
    VulkanSyncManager& operator=(const VulkanSyncManager&) = delete;

    VkSemaphore getAvailableSemaphore(uint32_t currentFrame) const noexcept { return m_imageAvailableSemaphores[currentFrame]; }
    VkSemaphore getFinishedSemaphore(uint32_t imageIndex)  const noexcept { return m_renderFinishedSemaphores[imageIndex]; }
    VkFence getInFlightFence(uint32_t currentFrame) const noexcept { return m_inFlightFences[currentFrame]; }
    VkFence* getInFlightFencePtr(uint32_t currentFrame) noexcept { return &m_inFlightFences[currentFrame]; }
private:
    VulkanDevice& m_device;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};