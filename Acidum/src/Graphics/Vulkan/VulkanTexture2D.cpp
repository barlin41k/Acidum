#include "Graphics/Vulkan/VulkanTexture2D.hpp"

#include <vulkan/vulkan.h>

#include "Graphics/Vulkan/VulkanBuffer.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanImage.hpp"
#include "Graphics/Vulkan/VulkanStagingManager.hpp"

namespace Acidum {
    
VulkanTexture2D::VulkanTexture2D(const VulkanDevice& device, VulkanStagingManager* stagingManager, const void* data, uint32_t width, uint32_t height, bool isSRGB)
    : m_device(device),
      m_stagingManager(stagingManager),
      m_width(width),
      m_height(height)
{
    createTextureImage(data, width, height, isSRGB);
    createSampler();
}

VulkanTexture2D::~VulkanTexture2D() {
    if (m_device.getLogicalDevice() != VK_NULL_HANDLE && m_sampler != VK_NULL_HANDLE)
        vkDestroySampler(m_device.getLogicalDevice(), m_sampler, nullptr);
}

void VulkanTexture2D::createTextureImage(const void* data, uint32_t width, uint32_t height, bool isSRGB) {
    VkDeviceSize bufferSize = width * height * 4;

    auto stagingBuffer = std::make_unique<VulkanBuffer>(
        m_device, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );

    stagingBuffer->map();
    stagingBuffer->copyTo(data, bufferSize);
    stagingBuffer->unmap();

    m_image = std::make_unique<VulkanImage>(
        m_device, width, height,
        isSRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    m_stagingManager->stageCopy(std::move(stagingBuffer), m_image->getImage(), width, height);
}

void VulkanTexture2D::createSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  
    vkCreateSampler(m_device.getLogicalDevice(), &samplerInfo, nullptr, &m_sampler);
}

} // namespace Acidum