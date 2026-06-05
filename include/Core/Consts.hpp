#pragma once

#include <cstdint>
#include <string>

#include "Core/Types.hpp"

namespace Consts {
    inline constexpr uint32_t WINDOW_WIDTH = 800;
    inline constexpr uint32_t WINDOW_HEIGHT = 600;
    inline constexpr std::string WINDOW_TITLE = "Vulkan Engine";

    inline constexpr APIType DEFAULT_API = APIType::Vulkan;
}