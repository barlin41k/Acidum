#include "Graphics/Vulkan/VulkanCommandBufferManager.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {

VulkanCommandBufferManager::VulkanCommandBufferManager(const VulkanDevice& device, uint32_t bufferCount)
    : m_device(device)
{
    QueueFamilyIndices queueFamilyIndices = m_device.getQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    ENGINE_VERIFY(vkCreateCommandPool(m_device.getLogicalDevice(), &poolInfo, nullptr, &m_commandPool) == VK_SUCCESS, "Failed to create command pool!");

    m_commandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) m_commandBuffers.size();

    ENGINE_VERIFY(vkAllocateCommandBuffers(m_device.getLogicalDevice(), &allocInfo, m_commandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");
}

VulkanCommandBufferManager::~VulkanCommandBufferManager() {
    vkDestroyCommandPool(m_device.getLogicalDevice(), m_commandPool, nullptr);
}

} // namespace Acidum