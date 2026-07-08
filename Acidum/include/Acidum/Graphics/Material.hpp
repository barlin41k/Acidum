#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {
    
struct Material {
    std::string vertShaderPath;
    std::string fragShaderPath;

    std::shared_ptr<ITexture2D> albedoTexture = nullptr;
    std::shared_ptr<ITexture2D> metallicRoughnessTexture = nullptr;
    std::shared_ptr<ITexture2D> normalTexture = nullptr;

    glm::vec4 baseColor = glm::vec4(1.0f);
    float roughness = 0.5f;
    float metallic = 0.0f;

    bool enableBlending = false;
    bool depthWrite = true;
};

} // namespace Acidum