#pragma once

#include <memory>

#include "Acidum/Core/Base/Types.hpp"
#include "Interfaces/IGraphicsAPI.hpp"

namespace Acidum {

class Window; // forward-declaration

namespace GraphicsFactory {
    std::unique_ptr<IGraphicsAPI> createAPI(APIType type, Window* window);
}

} // namespace Acidum