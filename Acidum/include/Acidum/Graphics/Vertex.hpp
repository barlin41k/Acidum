#pragma once

#include <glm/glm.hpp>

namespace Acidum {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec4 tangent;
};

} // namespace Acidum