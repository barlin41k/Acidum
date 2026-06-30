#include "Graphics/Vulkan/VulkanStagingManager.hpp"

#include "Graphics/Vulkan/VulkanLogger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {

VulkanStagingManager::VulkanStagingManager(const VulkanDevice& device)
    : m_device(device)
{
    createCommandPool();
    allocateCommandBuffers(m_commandBuffersCount);

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;

    ACIDUM_ASSERT(
        vkCreateFence(m_device.getLogicalDevice(), &fenceInfo, nullptr, &m_uploadFence) == VK_SUCCESS,
        "Failed to create uploadFence!"
    );
}

VulkanStagingManager::~VulkanStagingManager() {
    VkDevice device = m_device.getLogicalDevice();
    vkDestroyFence(device, m_uploadFence, nullptr);
    vkDestroyCommandPool(device, m_commandPool, nullptr);
}

void VulkanStagingManager::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = m_device.getQueueFamilies();

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    ACIDUM_ASSERT(
        vkCreateCommandPool(m_device.getLogicalDevice(), &poolInfo, nullptr, &m_commandPool) == VK_SUCCESS,
        "Failed to create command pool!"
    );
}

void VulkanStagingManager::allocateCommandBuffers(uint32_t bufferCount) {
    m_commandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = bufferCount;

    ACIDUM_ASSERT(
        vkAllocateCommandBuffers(m_device.getLogicalDevice(), &allocInfo, m_commandBuffers.data()) == VK_SUCCESS,
        "Failed to allocate command buffers!"
    );
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

void VulkanStagingManager::stageCopy(std::unique_ptr<VulkanBuffer> stagingBuffer, VkImage dstImage, uint32_t width, uint32_t height) {
    VkImageMemoryBarrier barrier {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = dstImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(
        m_commandBuffers[0],
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier
    );

    VkBufferImageCopy region {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(
        m_commandBuffers[0],
        stagingBuffer->getBuffer(),
        dstImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &region
    );

    VkImageMemoryBarrier shaderBarrier = barrier;
    shaderBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    shaderBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    shaderBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    shaderBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        m_commandBuffers[0],
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &shaderBarrier
    );
    
    m_stagingBuffers.push_back(std::move(stagingBuffer));
}

void VulkanStagingManager::submit() {
    vkEndCommandBuffer(m_commandBuffers[0]);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = m_commandBuffersCount;
    submitInfo.pCommandBuffers = &m_commandBuffers[0];

    vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, m_uploadFence);
}

void VulkanStagingManager::waitForUpload() {
    vkWaitForFences(m_device.getLogicalDevice(), 1, &m_uploadFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.getLogicalDevice(), 1, &m_uploadFence);
    m_stagingBuffers.clear();
}

} // namespace Acidum