#pragma once

#include <cstdint>

namespace Acidum {

using MouseCode = uint16_t;

namespace Mouse {
    enum : MouseCode {
        ButtonLeft = 0,
        ButtonRight = 1,
        ButtonMiddle = 2
    };
}

} // namespace Acidum