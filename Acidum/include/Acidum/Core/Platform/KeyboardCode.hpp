#pragma once

#include <cstdint>

namespace Acidum {

using KeyCode = uint16_t;

namespace Keyboard {
    enum : KeyCode {
        SPACE = 32,
        W = 87,
        A = 65,
        S = 83,
        D = 68
    };
}

} // namespace Acidum