#pragma once

#include <glm/glm.hpp>

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
};

} // namespace Acidum