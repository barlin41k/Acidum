#include "Graphics/Vulkan/VulkanImage.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include <vulkan/vulkan_core.h>

namespace Acidum {

VulkanImage::VulkanImage(const VulkanDevice& device, uint32_t width, uint32_t height, 
                VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
                VkImageAspectFlags aspectFlags)
                : m_device(device)
{
    createImage(width, height, format, tiling, usage);
    createImageView(format, aspectFlags);
}

VulkanImage::~VulkanImage() {
    vkDestroyImageView(m_device.getLogicalDevice(), m_imageView, nullptr);
    vmaDestroyImage(m_device.getAllocator(), m_image, m_allocation);
}

void VulkanImage::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) {
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

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    ACIDUM_ASSERT(
        vmaCreateImage(m_device.getAllocator(), &imageInfo, &allocInfo, &m_image, &m_allocation, nullptr) == VK_SUCCESS, 
        "Failed to create image via VMA!"
    );
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

    ACIDUM_ASSERT(
        vkCreateImageView(m_device.getLogicalDevice(), &viewInfo, nullptr, &m_imageView) == VK_SUCCESS, 
        "Failed to create image view!"
    );
    
}

} // namespace Acidum