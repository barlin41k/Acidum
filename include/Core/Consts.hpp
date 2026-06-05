#pragma once

#include <cstdint>
#include <string>

#include "Core/Types.hpp"

namespace Consts {
    inline constexpr uint32_t WINDOW_WIDTH = 800;
    inline constexpr uint32_t WINDOW_HEIGHT = 600;
    inline constexpr std::string WINDOW_TITLE = "Vulkan Engine";

    inline constexpr APIType DEFAULT_API = APIType::Vulkan;

    inline constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    inline const std::vector<Vertex> VERTICES = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    inline const std::vector<uint16_t> INDICES = {
        0, 1, 2, 2, 3, 0
    };
}