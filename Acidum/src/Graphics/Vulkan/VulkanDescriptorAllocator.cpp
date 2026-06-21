#include "Graphics/Vulkan/VulkanDescriptorAllocator.hpp"

#include <array>

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"

namespace Acidum {
    
VulkanDescriptorAllocator::VulkanDescriptorAllocator(const VulkanDevice& device) 
    : m_device(device) {}

bool VulkanDescriptorAllocator::allocate(VkDescriptorSetLayout layout, VkDescriptorSet* set) {
    if (currentPool == VK_NULL_HANDLE)
        currentPool = grabPool();

    VkDescriptorSetAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = currentPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkResult result = vkAllocateDescriptorSets(m_device.getLogicalDevice(), &allocInfo, set);
    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
        usedPools.push_back(currentPool);
        currentPool = grabPool();
        allocInfo.descriptorPool = currentPool;

        result = vkAllocateDescriptorSets(m_device.getLogicalDevice(), &allocInfo, set);
    }

    return result == VK_SUCCESS;
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
    for (auto pool : freePools)
        if (m_device.getLogicalDevice() != VK_NULL_HANDLE && pool != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(m_device.getLogicalDevice(), pool, nullptr);

    for (auto pool : usedPools)
        if (m_device.getLogicalDevice() != VK_NULL_HANDLE && pool != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(m_device.getLogicalDevice(), pool, nullptr);

    if (m_device.getLogicalDevice() != VK_NULL_HANDLE && currentPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(m_device.getLogicalDevice(), currentPool, nullptr);
    
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

    VkDescriptorPool pool {};
    ENGINE_VERIFY(
        vkCreateDescriptorPool(m_device.getLogicalDevice(), &poolInfo, nullptr, &pool) == VK_SUCCESS,
        "Failed to create descriptor pool!"
    );

    return pool;
}

} // namespace Acidum