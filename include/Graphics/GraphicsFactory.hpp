#pragma once

#include <memory>

#include "Core/Types.hpp"
#include "Interfaces/IGraphicsAPI.hpp"

class Window; // forward-declaration

namespace GraphicsFactory {
    std::unique_ptr<IGraphicsAPI> createAPI(APIType type, Window* window);
}