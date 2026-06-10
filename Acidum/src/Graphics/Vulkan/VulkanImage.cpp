#include "Graphics/Vulkan/VulkanImage.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include <vulkan/vulkan_core.h>

namespace Acidum {

VulkanImage::VulkanImage(const VulkanDevice& device, uint32_t width, uint32_t height, 
                VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
                VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags)
                : m_device(device)
{
    createImage(width, height, format, tiling, usage, properties);
    createImageView(format, aspectFlags);
}

VulkanImage::~VulkanImage() {
    if (m_imageView != VK_NULL_HANDLE)
        vkDestroyImageView(m_device.getLogicalDevice(), m_imageView, nullptr);
    if (m_image != VK_NULL_HANDLE)
        vkDestroyImage(m_device.getLogicalDevice(), m_image, nullptr);
    if (m_imageMemory != VK_NULL_HANDLE)
        vkFreeMemory(m_device.getLogicalDevice(), m_imageMemory, nullptr);
}

void VulkanImage::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkImageCreateInfo imageInfo {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    ENGINE_VERIFY(vkCreateImage(m_device.getLogicalDevice(), &imageInfo, nullptr, &m_image) == VK_SUCCESS, "Failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device.getLogicalDevice(), m_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    allocInfo.memoryTypeIndex = m_device.findMemoryType(memRequirements.memoryTypeBits, properties);
    ENGINE_VERIFY(vkAllocateMemory(m_device.getLogicalDevice(), &allocInfo, nullptr, &m_imageMemory) == VK_SUCCESS,
        "Failed to allocate image memory!");
    vkBindImageMemory(m_device.getLogicalDevice(), m_image, m_imageMemory, 0);
}

void VulkanImage::createImageView(VkFormat format, VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewInfo {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    ENGINE_VERIFY(
        vkCreateImageView(m_device.getLogicalDevice(), &viewInfo, nullptr, &m_imageView) == VK_SUCCESS, 
        "Failed to create image view!"
    );
    
}

} // namespace Acidum