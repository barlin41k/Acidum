#pragma once

#include <glm/glm.hpp>

struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

enum class APIType {
    Vulkan
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
};