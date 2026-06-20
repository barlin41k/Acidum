#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <cstdint>

namespace Acidum {

// forward-declaration
class VulkanDevice;

class VulkanImage {
public:
    VulkanImage(const VulkanDevice& device, uint32_t width, uint32_t height, 
                VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
                VkImageAspectFlags aspectFlags);
    ~VulkanImage();

    VulkanImage(const VulkanImage&) = delete;
    VulkanImage& operator=(const VulkanImage&) = delete;
    VulkanImage(VulkanImage&&) = delete;
    VulkanImage& operator=(VulkanImage&&) = delete;

    VkImage getImage() const { return m_image; }
    VkImageView getImageView() const { return m_imageView; }
private:
    const VulkanDevice& m_device;
    VmaAllocation m_allocation;

    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
};

} // namespace Acidum