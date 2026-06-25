#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Acidum/Core/Base/Types.hpp"

// forward-declaration
namespace tinygltf {
    class Node;
    class Model;
}

namespace Acidum {

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    std::string textureName;
    std::vector<uint8_t> embeddedImage;

    std::string metallicRoughnessTextureName;
    std::vector<uint8_t> embeddedMetallicRoughnessImage;

    std::string normalTextureName;
    std::vector<uint8_t> embeddedNormalImage;

    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    float roughness = 0.5f;
    float metallic = 0.0f;

    bool isTransparent = false;
};
    
class ModelLoader {
public:
    static std::vector<MeshData> load(const std::string& path);
private:
    static void processNode(const tinygltf::Node& node, const tinygltf::Model& model, const glm::mat4& parentMatrix, std::vector<MeshData>& allMeshes);
};

} // namespace Acidum