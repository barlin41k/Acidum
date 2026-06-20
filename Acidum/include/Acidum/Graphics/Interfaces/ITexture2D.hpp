#pragma once

#include <cstdint>

namespace Acidum {
    
class ITexture2D {
public:
    virtual ~ITexture2D() = default;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
};

} // namespace Acidum