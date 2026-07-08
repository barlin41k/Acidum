#include "Graphics/Vulkan/VulkanMesh.hpp"

namespace Acidum {

void VulkanMesh::bind(VkCommandBuffer commandBuffer) const {
    VkBuffer vertexBuffers[] = { m_vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void VulkanMesh::draw(VkCommandBuffer commandBuffer) const {
    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}

void VulkanMesh::createIndexBuffer(const VulkanDevice& device, VulkanStagingManager* stagingManager, const std::vector<uint32_t>& indices) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    auto stagingBuffer = std::make_unique<VulkanBuffer>(
        device, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    stagingBuffer->map();
    stagingBuffer->copyTo((void*)indices.data(), bufferSize);
    stagingBuffer->unmap();

    m_indexBuffer = std::make_unique<VulkanBuffer>(device, bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    stagingManager->stageCopy(std::move(stagingBuffer), m_indexBuffer->getBuffer(), bufferSize);
}

VkVertexInputBindingDescription VulkanMesh::getBindingDescription(const VertexLayout& layout) {
    VkVertexInputBindingDescription bindingDescription {};
    bindingDescription.binding = 0;
    bindingDescription.stride = layout.getStride();
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> VulkanMesh::getAttributeDescriptions(const VertexLayout& layout) {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    const auto& elements = layout.getElements();

    for (uint32_t i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];

        VkVertexInputAttributeDescription attribute {};
        attribute.binding = 0;
        attribute.location = i;
        attribute.format = ShaderDataTypeToVulkanFormat(element.type);
        attribute.offset = element.offset;

        attributeDescriptions.push_back(attribute);
    }

    return attributeDescriptions;
}

} // namespace Acidum