#include "Graphics/Vulkan/VulkanSyncManager.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {
VulkanSyncManager::VulkanSyncManager(VulkanDevice& device, uint32_t maxFramesInFlight, uint32_t imageCount) 
    : m_device(device) {
    m_imageAvailableSemaphores.resize(maxFramesInFlight);
    m_renderFinishedSemaphores.resize(imageCount);
    m_inFlightFences.resize(maxFramesInFlight);
    m_imagesInFlight.resize(imageCount, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < maxFramesInFlight; i++)
        ENGINE_VERIFY(
            vkCreateSemaphore(m_device.getLogicalDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) == VK_SUCCESS, 
            "Failed to create ImageAvailable semaphore for frame {}!", i
        );

    for (size_t i = 0; i < maxFramesInFlight; i++)
        ENGINE_VERIFY(
            vkCreateFence(m_device.getLogicalDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) == VK_SUCCESS, 
            "Failed to create InFlight fence for frame {}!", i
        );

    for (size_t i = 0; i < imageCount; i++)
        ENGINE_VERIFY(
            vkCreateSemaphore(m_device.getLogicalDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) == VK_SUCCESS, 
            "Failed to create RenderFinished semaphore for image {}!", i
        );
}

VulkanSyncManager::~VulkanSyncManager() {
    for (auto semaphore : m_renderFinishedSemaphores)
        vkDestroySemaphore(m_device.getLogicalDevice(), semaphore, nullptr);

    for (auto semaphore : m_imageAvailableSemaphores)
        vkDestroySemaphore(m_device.getLogicalDevice(), semaphore, nullptr);

    for (auto fence : m_inFlightFences)
        vkDestroyFence(m_device.getLogicalDevice(), fence, nullptr);
}
} // namespace Acidum