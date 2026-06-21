#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Acidum {

// forward-declaration
class VulkanDevice;
    
class VulkanDescriptorAllocator {
public:
    VulkanDescriptorAllocator(const VulkanDevice& device);
    ~VulkanDescriptorAllocator() = default;

    bool allocate(VkDescriptorSetLayout layout, VkDescriptorSet* set);
    void resetPools();
    void cleanup();
private:
    const VulkanDevice& m_device;

    VkDescriptorPool currentPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorPool> usedPools;
    std::vector<VkDescriptorPool> freePools;

    VkDescriptorPool grabPool();
    VkDescriptorPool createPool();
};

} // namespace Acidum