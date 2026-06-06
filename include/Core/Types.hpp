#pragma once

#include <glm/glm.hpp>

enum class APIType {
    Vulkan
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};