#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Acidum/Graphics/Interfaces/IMesh.hpp"

namespace Acidum {

static constexpr size_t InvalidIndex = static_cast<size_t>(-1);

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

    size_t findNodeIndex(const std::string& name) const {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].name == name)
                return i;
        }
        return InvalidIndex;
    }

    bool setMaterialForNode(const std::string& name, std::shared_ptr<Material> material) {
        size_t index = findNodeIndex(name);
        if (index != InvalidIndex && nodes[index].mesh) {
            nodes[index].mesh->setMaterial(material);
            return true;
        }
        return false;
    }
};

} // namespace Acidum