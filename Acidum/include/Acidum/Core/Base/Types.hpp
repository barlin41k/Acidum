#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Acidum/Graphics/Interfaces/IMesh.hpp"

namespace Acidum {

struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

enum class APIType {
    Vulkan
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec4 tangent;
};

struct MeshNode {
    std::shared_ptr<IMesh> mesh;
    glm::mat4 localTransform { 1.0f };
    std::string name;
};

struct Model {
    std::vector<MeshNode> nodes;

    int findNodeIndex(const std::string& name) const {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].name == name)
                return static_cast<int>(i);
        }
        return -1;
    }
};

} // namespace Acidum