#pragma once

#include <cstdint>
#include <memory>

#include "Acidum/Graphics/Material.hpp"

namespace Acidum {

class IMesh {
public:
    virtual ~IMesh() = default;

    virtual uint32_t getVertexCount() const = 0;
    virtual uint32_t getIndexCount() const = 0;

    virtual void setMaterial(std::shared_ptr<Material> material) = 0;
    virtual Material* getMaterial() const = 0;
};

} // namespace Acidum