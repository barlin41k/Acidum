#pragma once

#include <glm/glm.hpp>

namespace Acidum {

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
};

} // namespace Acidum