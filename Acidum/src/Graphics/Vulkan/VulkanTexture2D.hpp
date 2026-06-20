#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"
#include "Graphics/Vulkan/VulkanImage.hpp"

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanStagingManager;
    
class VulkanTexture2D : public ITexture2D {
public:
    VulkanTexture2D(const VulkanDevice& device, VulkanStagingManager* stagingManager, const void* data, uint32_t width, uint32_t height);
    ~VulkanTexture2D();

    uint32_t getWidth() const override { return m_width; }
    uint32_t getHeight() const override { return m_height; }

    VkImageView getImageView() const { return m_image->getImageView(); };
    VkSampler getSampler() const { return m_sampler; };
private:
    const VulkanDevice& m_device;
    VulkanStagingManager* m_stagingManager;

    std::unique_ptr<VulkanImage> m_image;
    VkSampler m_sampler = VK_NULL_HANDLE;

    uint32_t m_width = 0, m_height = 0;

    void createTextureImage(const void* data, uint32_t width, uint32_t height);
    void createSampler();
};

} // namespace Acidum