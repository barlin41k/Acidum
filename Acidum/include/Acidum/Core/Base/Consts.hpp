#pragma once

#include <cstdint>

#include "Acidum/Core/Base/Types.hpp"

namespace Acidum {

namespace Consts {
    inline constexpr const char* ENGINE_NAME = "Acidum Engine";
    inline constexpr uint32_t ENGINE_DEFAULT_WIDTH = 800;
    inline constexpr uint32_t ENGINE_DEFAULT_HEIGHT = 600;
    inline constexpr APIType ENGINE_DEFAULT_API = APIType::Vulkan;
    inline constexpr Version ENGINE_VERSION = { 0, 2, 1 };

    inline constexpr int MAX_FRAMES_IN_FLIGHT = 2;
}

} // namespace Acidum