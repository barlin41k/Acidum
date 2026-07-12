#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {

struct MaterialConfig {
    std::string vertShaderPath;
    std::string fragShaderPath;

    bool enableBlending = false;
    bool enableDepthWrite = true;
};
    
struct Material {
    MaterialConfig config;
    
    std::shared_ptr<ITexture2D> albedoTexture = nullptr;
    std::shared_ptr<ITexture2D> metallicRoughnessTexture = nullptr;
    std::shared_ptr<ITexture2D> normalTexture = nullptr;

    glm::vec4 baseColor = glm::vec4(1.0f);
    float roughness = 0.5f;
    float metallic = 0.0f;
};

} // namespace Acidum