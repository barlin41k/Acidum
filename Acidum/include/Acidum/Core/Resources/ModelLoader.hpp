#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Acidum/Core/Base/Types.hpp"

namespace Acidum {

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string textureName;
};
    
class ModelLoader {
public:
    static std::vector<MeshData> load(const std::string& path);
};

} // namespace Acidum