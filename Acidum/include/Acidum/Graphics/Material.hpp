#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {
    
struct Material {
    const std::string vertShaderPath;
    const std::string fragShaderPath;

    std::shared_ptr<ITexture2D> albedoTexture = nullptr;

    glm::vec3 baseColor { 1.0f, 1.0f, 1.0f };

    bool enableBlending = false;
    bool depthWrite = true;

    Material(const std::string& vert, const std::string& frag, bool inEnableBlending, bool inDepthWrite)
        : vertShaderPath(vert),
          fragShaderPath(frag),
          enableBlending(inEnableBlending),
          depthWrite(inDepthWrite) {}
};

} // namespace Acidum