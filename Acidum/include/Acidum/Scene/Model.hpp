#pragma once

#include <memory>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Graphics/Interfaces/IMesh.hpp"

namespace Acidum {

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