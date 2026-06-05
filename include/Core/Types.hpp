#pragma once

#include <glm/glm.hpp>

enum class APIType {
    Vulkan
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};