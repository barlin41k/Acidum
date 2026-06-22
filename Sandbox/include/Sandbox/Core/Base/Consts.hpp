#pragma once

#include <cstdint>

namespace Sandbox {

namespace Consts {
    inline constexpr float CAMERA_FOV = 45.0f;
    inline constexpr float CAMERA_NEAR_CLIP = 0.1f;
    inline constexpr float CAMERA_FAR_CLIP = 100.0f;
    inline constexpr float CAMERA_SPEED = 2.0f;

    inline constexpr uint32_t WINDOW_WIDTH = 1280;
    inline constexpr uint32_t WINDOW_HEIGHT = 720;
    inline constexpr const char* WINDOW_TITLE = "Sandbox";
}

} // namespace Sandbox