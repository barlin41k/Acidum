#pragma once

#include <initializer_list>
#include <cstdint>
#include <string>
#include <vector>

namespace Acidum {

enum class ShaderDataType {
    None = 0,
    Float, Float2, Float3, Float4,
    Int, Int2, Int3, Int4,
    Mat3, Mat4,
    Bool
};

constexpr uint32_t ShaderDataTypeSize(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:  return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Int:    return 4;
        case ShaderDataType::Int2:   return 4 * 2;
        case ShaderDataType::Int3:   return 4 * 3;
        case ShaderDataType::Int4:   return 4 * 4;
        case ShaderDataType::Mat3:   return 4 * 3 * 3;
        case ShaderDataType::Mat4:   return 4 * 4 * 4;
        case ShaderDataType::Bool:   return 1;
        case ShaderDataType::None:   break;
    }

    return 0;
}

struct BufferElement {
    ShaderDataType type = ShaderDataType::None;
    std::string name;
    uint32_t size = 0;
    uint32_t offset = 0;
    bool normalized = false;
};

class VertexLayout {
public:
    VertexLayout() = default;
    VertexLayout(std::initializer_list<BufferElement> elements);

    static const VertexLayout& GetDefaultPBRLayout();

    uint32_t getStride() const { return m_stride; }
    const std::vector<BufferElement>& getElements() const { return m_elements; }

    std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }
private:
    std::vector<BufferElement> m_elements;
    uint32_t m_stride = 0;

    void calculateOffsetsAndStride();
};

} // namespace Acidum