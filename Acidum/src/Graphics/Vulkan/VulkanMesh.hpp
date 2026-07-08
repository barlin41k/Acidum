#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#include "Acidum/Graphics/Interfaces/IMesh.hpp"
#include "Acidum/Graphics/Material.hpp"
#include "Acidum/Graphics/VertexLayout.hpp"
#include "Graphics/Vulkan/VulkanBuffer.hpp"
#include "Graphics/Vulkan/VulkanStagingManager.hpp"

namespace Acidum {

// forward-declaration
class VulkanDevice;
class VulkanStagingManager;

constexpr VkFormat ShaderDataTypeToVulkanFormat(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:  return VK_FORMAT_R32_SFLOAT;
        case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
        case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
        case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ShaderDataType::Int:    return VK_FORMAT_R32_SINT;
        case ShaderDataType::Int2:   return VK_FORMAT_R32G32_SINT;
        case ShaderDataType::Int3:   return VK_FORMAT_R32G32B32_SINT;
        case ShaderDataType::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
        case ShaderDataType::Bool:   return VK_FORMAT_R8_UINT;
        default:                     break;
    }

    return VK_FORMAT_UNDEFINED;
}

class VulkanMesh : public IMesh {
public:
    template<typename VertexType>
    VulkanMesh(
        const VulkanDevice& device, VulkanStagingManager* stagingManager,
        const std::vector<VertexType>& vertices, const std::vector<uint32_t>& indices, const VertexLayout& layout=VertexLayout::GetDefaultPBRLayout()
    );
    ~VulkanMesh() = default;

    VulkanMesh(const VulkanMesh&) = delete;
    VulkanMesh& operator=(const VulkanMesh&) = delete;

    uint32_t getVertexCount() const override { return m_vertexCount; };
    uint32_t getIndexCount() const override { return m_indexCount; };
    const VertexLayout& getLayout() const override { return m_layout;}

    void setMaterial(std::shared_ptr<Material> material) override { m_material = std::move(material); };
    Material* getMaterial() const override { return m_material.get(); };

    void bind(VkCommandBuffer commandBuffer) const;
    void draw(VkCommandBuffer commandBuffer) const;

    static VkVertexInputBindingDescription getBindingDescription(const VertexLayout& layout);
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(const VertexLayout& layout);
private:
    std::unique_ptr<VulkanBuffer> m_vertexBuffer;
    std::unique_ptr<VulkanBuffer> m_indexBuffer;
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;

    VertexLayout m_layout;
    std::shared_ptr<Material> m_material;

    template<typename VertexType>
    void createVertexBuffer(const VulkanDevice& device, VulkanStagingManager* stagingManager, const std::vector<VertexType>& vertices);
    void createIndexBuffer(const VulkanDevice& device, VulkanStagingManager* stagingManager, const std::vector<uint32_t>& indices);
};

// funcs with templates

template<typename VertexType>
VulkanMesh::VulkanMesh(
    const VulkanDevice& device, VulkanStagingManager* stagingManager,
    const std::vector<VertexType>& vertices, const std::vector<uint32_t>& indices, const VertexLayout& layout
)
    : m_vertexCount(static_cast<uint32_t>(vertices.size())),
      m_indexCount(static_cast<uint32_t>(indices.size())),
      m_layout(layout)
{
    createVertexBuffer(device, stagingManager, vertices);
    createIndexBuffer(device, stagingManager, indices);
}

template<typename VertexType>
void VulkanMesh::createVertexBuffer(const VulkanDevice& device, VulkanStagingManager* stagingManager, const std::vector<VertexType>& vertices) {
    VkDeviceSize bufferSize = sizeof(VertexType) * vertices.size();

    auto stagingBuffer = std::make_unique<VulkanBuffer>(
        device, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    stagingBuffer->map();
    stagingBuffer->copyTo((void*)vertices.data(), bufferSize);
    stagingBuffer->unmap();

    m_vertexBuffer = std::make_unique<VulkanBuffer>(
        device, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    stagingManager->stageCopy(std::move(stagingBuffer), m_vertexBuffer->getBuffer(), bufferSize);
}

} // namespace Acidum