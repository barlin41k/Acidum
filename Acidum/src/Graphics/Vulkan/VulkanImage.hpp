#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Acidum {

// forward-declaration
class VulkanDevice;

class VulkanImage {
public:
    VulkanImage(const VulkanDevice& device, uint32_t width, uint32_t height, 
                VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
                VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags);
    ~VulkanImage();

    VulkanImage(const VulkanImage&) = delete;
    VulkanImage& operator=(const VulkanImage&) = delete;

    VkImage getImage() const { return m_image; }
    VkImageView getImageView() const { return m_imageView; }
private:
    const VulkanDevice& m_device;
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
};

} // namespace Acidum