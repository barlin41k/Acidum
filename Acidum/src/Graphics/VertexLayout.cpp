#include "Acidum/Graphics/VertexLayout.hpp"

namespace Acidum {

VertexLayout::VertexLayout(std::initializer_list<BufferElement> elements)
    : m_elements(elements)
{
    calculateOffsetsAndStride();
}

const VertexLayout& VertexLayout::GetDefaultPBRLayout() {
    static VertexLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float3, "a_Color" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float3, "a_Normal" },
        { ShaderDataType::Float4, "a_Tangent" }
    };

    return layout;
}

void VertexLayout::calculateOffsetsAndStride() {
    uint32_t offset = 0;
    m_stride = 0;

    for (auto& element : m_elements) {
        element.size = ShaderDataTypeSize(element.type);
        element.offset = offset;
        offset += element.size;
        m_stride += element.size;
    }
}

} // namespace Acidum