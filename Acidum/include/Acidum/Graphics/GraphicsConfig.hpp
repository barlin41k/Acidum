#pragma once

#include "glm/ext/vector_float4.hpp"

#include <string>

namespace Acidum {

enum class GPUPreference {
    HighPerformance, LowPower, Manual
};

struct GPUAdapterInfo {
    uint32_t id = 0;
    std::string name;
    uint64_t vramSizeMB = 0;
    bool isDiscrete = true;
};

struct GraphicsConfig {
    bool enableVSync = false;

    GPUPreference gpuPreference = GPUPreference::HighPerformance;
    std::string preferredDeviceName = "";

    bool supportWireframe = true;

    glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
};

} // namespace Acidum