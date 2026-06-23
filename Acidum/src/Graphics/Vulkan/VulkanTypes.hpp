#pragma once

#include <glm/glm.hpp>

namespace Acidum {

struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 lightDir;
};

} // namespace Acidum