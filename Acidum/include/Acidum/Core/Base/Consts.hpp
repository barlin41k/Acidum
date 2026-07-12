#pragma once

#include "Acidum/Core/Base/Types.hpp"

namespace Acidum {

namespace Consts {
    
    namespace ENGINE {
        inline constexpr const char* NAME = "Acidum Engine";
        inline constexpr Version VERSION = { 0, 2, 12 };
    } // namespace ENGINE

    namespace WINDOW {
        inline constexpr uint32_t DEFAULT_WIDTH = 1280;
        inline constexpr uint32_t DEFAULT_HEIGHT = 720;
    } // namespace WINDOW

    namespace RENDERER {
        inline constexpr APIType DEFAULT_API_TYPE = APIType::Vulkan;
        inline constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    } // namespace RENDERER

} // namespace Consts

} // namespace Acidum