#pragma once

#include <glm/glm.hpp>

namespace Acidum {

struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec3 viewPos;
};

struct PushContants {
    glm::mat4 model;
    glm::vec4 baseColor;
    float roughness;
    float metallic;
};

} // namespace Acidum