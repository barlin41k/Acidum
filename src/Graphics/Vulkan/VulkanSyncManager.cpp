#include "Graphics/Vulkan/VulkanSyncManager.hpp"

#include <stdexcept>

#include "Graphics/Vulkan/VulkanDevice.hpp"

VulkanSyncManager::VulkanSyncManager(VulkanDevice& device, uint32_t maxFramesInFlight, uint32_t imageCount) 
    : m_device(device) {
    m_imageAvailableSemaphores.resize(maxFramesInFlight);
    m_renderFinishedSemaphores.resize(imageCount);
    m_inFlightFences.resize(maxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        if (vkCreateSemaphore(m_device.getLogicalDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device.getLogicalDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create frame sync objects!");
        }
    }

    for (size_t i = 0; i < imageCount; i++) {
        if (vkCreateSemaphore(m_device.getLogicalDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create render finished semaphores!");
    }
}

VulkanSyncManager::~VulkanSyncManager() {
    for (auto semaphore : m_renderFinishedSemaphores)
        vkDestroySemaphore(m_device.getLogicalDevice(), semaphore, nullptr);
    for (size_t i = 0; i < m_imageAvailableSemaphores.size(); i++) {
        vkDestroySemaphore(m_device.getLogicalDevice(), m_imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_device.getLogicalDevice(), m_inFlightFences[i], nullptr);
    }
}