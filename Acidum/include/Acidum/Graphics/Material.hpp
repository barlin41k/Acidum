#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {
    
struct Material {
    std::shared_ptr<ITexture2D> albedoTexture = nullptr;
    glm::vec3 baseColor { 1.0f, 1.0f, 1.0f };
};

} // namespace Acidum