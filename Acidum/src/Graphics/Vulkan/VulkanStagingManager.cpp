#include "Graphics/Vulkan/VulkanStagingManager.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include <vulkan/vulkan_core.h>

namespace Acidum {

VulkanStagingManager::VulkanStagingManager(const VulkanDevice& device)
    : m_device(device)
{
    createCommandPool();
    allocateCommandBuffers(m_commandBuffersCount);

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    ENGINE_VERIFY(
        vkCreateFence(m_device.getLogicalDevice(), &fenceInfo, nullptr, &m_uploadFence) == VK_SUCCESS,
        "Failed to create uploadFence!"
    );
}

VulkanStagingManager::~VulkanStagingManager() {
    vkDestroyFence(m_device.getLogicalDevice(), m_uploadFence, nullptr);
    vkDestroyCommandPool(m_device.getLogicalDevice(), m_commandPool, nullptr);
}

void VulkanStagingManager::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = m_device.getQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();

    ENGINE_VERIFY(vkCreateCommandPool(m_device.getLogicalDevice(), &poolInfo, nullptr, &m_commandPool) == VK_SUCCESS, "Failed to create command pool!");
}

void VulkanStagingManager::allocateCommandBuffers(uint32_t bufferCount) {
    m_commandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = bufferCount;

    ENGINE_VERIFY(vkAllocateCommandBuffers(m_device.getLogicalDevice(), &allocInfo, m_commandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");
}

void VulkanStagingManager::begin() {
    vkResetCommandPool(m_device.getLogicalDevice(), m_commandPool, 0);

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_commandBuffers[0], &beginInfo);
}

void VulkanStagingManager::stageCopy(std::unique_ptr<VulkanBuffer> stagingBuffer, VkBuffer dst, VkDeviceSize size) {
    VkBufferCopy copyRegion {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;

    vkCmdCopyBuffer(m_commandBuffers[0], stagingBuffer->getBuffer(), dst, 1, &copyRegion);
    m_stagingBuffers.push_back(std::move(stagingBuffer));
}

void VulkanStagingManager::submit() {
    vkEndCommandBuffer(m_commandBuffers[0]);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = m_commandBuffersCount;
    submitInfo.pCommandBuffers = &m_commandBuffers[0];

    vkQueueSubmit(m_device.getTransferQueue(), 1, &submitInfo, m_uploadFence);
}

void VulkanStagingManager::waitForUpload() {
    vkWaitForFences(m_device.getLogicalDevice(), 1, &m_uploadFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.getLogicalDevice(), 1, &m_uploadFence);
    m_stagingBuffers.clear();
}

} // namespace Acidum