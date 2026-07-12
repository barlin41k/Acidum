#pragma once

#include <cstdint>

#include "Acidum/Core/Base/Types.hpp"

namespace Sandbox {

namespace Consts {

    namespace APPLICATION {
        inline constexpr Acidum::Version VERSION = { 0, 2, 0 };
    } // namespace APPLICATION

    namespace WINDOW {
        inline constexpr uint32_t WIDTH = 1280;
        inline constexpr uint32_t HEIGHT = 720;
        inline constexpr const char* TITLE = "Sandbox";
    } // namespace WINDOW

    namespace RENDERER {

        namespace CAMERA {
            inline constexpr float FOV = 45.0f;
            inline constexpr float NEAR_CLIP = 0.1f;
            inline constexpr float FAR_CLIP = 100.0f;
            inline constexpr float SPEED = 2.0f;
        } // namespace CAMERA

    } // namespace RENDERER
}

} // namespace Sandbox