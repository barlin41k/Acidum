#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#include "Acidum/Graphics/Interfaces/IMesh.hpp"
#include "Graphics/Vulkan/VulkanBuffer.hpp"

namespace Acidum {
class VulkanDevice; // forward-declaration

class VulkanMesh : public IMesh {
public:
    template<typename VertexType>
    VulkanMesh(VulkanDevice& device, VkCommandPool commandPool, const std::vector<VertexType>& vertices, const std::vector<uint32_t>& indices);
    ~VulkanMesh() = default;

    VulkanMesh(const VulkanMesh&) = delete;
    VulkanMesh& operator=(const VulkanMesh&) = delete;

    uint32_t getVertexCount() const override { return m_vertexCount; };
    uint32_t getIndexCount() const override { return m_indexCount; };

    void bind(VkCommandBuffer commandBuffer) const;
    void draw(VkCommandBuffer commandBuffer) const;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
private:
    std::unique_ptr<VulkanBuffer> m_vertexBuffer;
    std::unique_ptr<VulkanBuffer> m_indexBuffer;
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;

    template<typename VertexType>
    void createVertexBuffer(VulkanDevice& device, VkCommandPool commandPool, const std::vector<VertexType>& vertices);
    void createIndexBuffer(VulkanDevice& device, VkCommandPool commandPool, const std::vector<uint32_t>& indices);
};

// templates
template<typename VertexType>
VulkanMesh::VulkanMesh(VulkanDevice& device, VkCommandPool commandPool, const std::vector<VertexType>& vertices, const std::vector<uint32_t>& indices) 
    : m_vertexCount(static_cast<uint32_t>(vertices.size())), m_indexCount(static_cast<uint32_t>(indices.size())) {
    createVertexBuffer(device, commandPool, vertices);
    createIndexBuffer(device, commandPool, indices);
}

template<typename VertexType>
void VulkanMesh::createVertexBuffer(VulkanDevice& device, VkCommandPool commandPool, const std::vector<VertexType>& vertices) {
    VkDeviceSize bufferSize = sizeof(VertexType) * vertices.size();

    VulkanBuffer stagingBuffer(device, bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.copyTo((void*)vertices.data(), bufferSize);

    m_vertexBuffer = std::make_unique<VulkanBuffer>(device, bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VulkanBuffer::copyBuffer(device, commandPool, stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), bufferSize);
}
} // namespace Acidum