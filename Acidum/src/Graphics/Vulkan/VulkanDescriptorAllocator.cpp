#include "Graphics/Vulkan/VulkanDescriptorAllocator.hpp"

#include <array>

#include "Graphics/Vulkan/VulkanLogger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {
    
VulkanDescriptorAllocator::VulkanDescriptorAllocator(const VulkanDevice& device) 
    : m_device(device) {}

bool VulkanDescriptorAllocator::allocate(VkDescriptorSetLayout layout, VkDescriptorSet* set) {
    if (currentPool == VK_NULL_HANDLE) currentPool = grabPool();

    VkDescriptorSetAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = currentPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    for (int attempts = 0; attempts < 2; ++attempts) {
        VkResult result = vkAllocateDescriptorSets(m_device.getLogicalDevice(), &allocInfo, set);

        if (result == VK_SUCCESS) return true;

        if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
            usedPools.push_back(currentPool);
            currentPool = grabPool();
            allocInfo.descriptorPool = currentPool;
        } else return false;
    }

    return false;
}

void VulkanDescriptorAllocator::resetPools() {
    for (auto pool : usedPools) {
        vkResetDescriptorPool(m_device.getLogicalDevice(), pool, 0);
        freePools.push_back(pool);
    }
    usedPools.clear();

    if (currentPool != VK_NULL_HANDLE) {
        vkResetDescriptorPool(m_device.getLogicalDevice(), currentPool, 0);
        freePools.push_back(currentPool);
        currentPool = VK_NULL_HANDLE;
    }
}

void VulkanDescriptorAllocator::cleanup() {
    VkDevice device = m_device.getLogicalDevice();

    for (auto pool : freePools)
        vkDestroyDescriptorPool(device, pool, nullptr);
    for (auto pool : usedPools)
        vkDestroyDescriptorPool(device, pool, nullptr);
    if (currentPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(device, currentPool, nullptr);
    
    freePools.clear();
    usedPools.clear();
    currentPool = VK_NULL_HANDLE;
}

VkDescriptorPool VulkanDescriptorAllocator::grabPool() {
    if (!freePools.empty()) {
        VkDescriptorPool pool = freePools.back();
        freePools.pop_back();
        return pool;
    }
    
    return createPool();
}

VkDescriptorPool VulkanDescriptorAllocator::createPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes = {{
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 }
    }};

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 2000;
    poolInfo.flags = 0;

    VkDescriptorPool pool = VK_NULL_HANDLE;
    ACIDUM_ASSERT(
        vkCreateDescriptorPool(m_device.getLogicalDevice(), &poolInfo, nullptr, &pool) == VK_SUCCESS,
        "Failed to create descriptor pool!"
    );

    return pool;
}

} // namespace Acidum