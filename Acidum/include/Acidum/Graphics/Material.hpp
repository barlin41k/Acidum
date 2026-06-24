#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {
    
struct Material {
    const std::string vertShaderPath;
    const std::string fragShaderPath;

    std::shared_ptr<ITexture2D> albedoTexture = nullptr;
    std::shared_ptr<ITexture2D> metallicRoughnessTexture = nullptr;

    glm::vec4 baseColor = glm::vec4(1.0f);
    float roughness = 0.5f;
    float metallic = 0.0f;

    bool enableBlending = false;
    bool depthWrite = true;

    Material(
        const std::string& vert, const std::string& frag,
        const glm::vec4& inBaseColor,
        float inRoughness, float inMetallic,
        bool inEnableBlending,
        bool inDepthWrite
    )
        : vertShaderPath(vert),
          fragShaderPath(frag),
          baseColor(inBaseColor),
          roughness(inRoughness),
          metallic(inMetallic),
          enableBlending(inEnableBlending),
          depthWrite(inDepthWrite) {}
};

} // namespace Acidum