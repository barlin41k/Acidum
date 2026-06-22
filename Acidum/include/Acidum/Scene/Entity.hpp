#pragma once

#include <memory>

#include "Acidum/Graphics/Interfaces/IMesh.hpp"

namespace Acidum {
    
struct Entity {
    std::shared_ptr<IMesh> mesh = nullptr;
    glm::mat4 transform { 1.0f };
};

} // namespace Acidum