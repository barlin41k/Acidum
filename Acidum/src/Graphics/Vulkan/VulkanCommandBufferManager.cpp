#include "Graphics/Vulkan/VulkanCommandBufferManager.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Acidum {

VulkanCommandBufferManager::VulkanCommandBufferManager(const VulkanDevice& device, uint32_t bufferCount)
    : m_device(device)
{
    createCommandPool();
    allocateCommandBuffers(bufferCount);
}

VulkanCommandBufferManager::~VulkanCommandBufferManager() {
    VkDevice device = m_device.getLogicalDevice();
    
    vkDestroyCommandPool(device, m_commandPool, nullptr);
}

void VulkanCommandBufferManager::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = m_device.getQueueFamilies();

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    ACIDUM_ASSERT(
        vkCreateCommandPool(m_device.getLogicalDevice(), &poolInfo, nullptr, &m_commandPool) == VK_SUCCESS,
        "Failed to create command pool!"
    );
}

void VulkanCommandBufferManager::allocateCommandBuffers(uint32_t bufferCount) {
    m_commandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    ACIDUM_ASSERT(
        vkAllocateCommandBuffers(m_device.getLogicalDevice(), &allocInfo, m_commandBuffers.data()) == VK_SUCCESS,
        "Failed to allocate command buffers!"
    );
}

} // namespace Acidum