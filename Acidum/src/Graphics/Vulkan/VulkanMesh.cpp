#include "Graphics/Vulkan/VulkanMesh.hpp"

#include "Acidum/Core/Base/Types.hpp"

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

void VulkanMesh::createIndexBuffer(const VulkanDevice& device, VkCommandPool commandPool, const std::vector<uint32_t>& indices) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VulkanBuffer stagingBuffer(device, bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.copyTo((void*)indices.data(), bufferSize);

    m_indexBuffer = std::make_unique<VulkanBuffer>(device, bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VulkanBuffer::copyBuffer(device, commandPool, stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);
}

VkVertexInputBindingDescription VulkanMesh::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> VulkanMesh::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    
    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    // Color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

} // namespace Acidum