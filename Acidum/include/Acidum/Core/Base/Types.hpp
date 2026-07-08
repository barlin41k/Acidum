#pragma once

#include <cstddef>
#include <cstdint>

namespace Acidum {

static constexpr size_t InvalidIndex = static_cast<size_t>(-1);

struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

enum class APIType {
    Vulkan
};

} // namespace Acidum