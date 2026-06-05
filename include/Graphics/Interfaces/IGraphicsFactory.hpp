#pragma once

#include <memory>

#include "Core/Types.hpp"
#include "IGraphicsAPI.hpp"

class Window; // forward-declaration

class IGraphicsFactory {
public:
    virtual ~IGraphicsFactory() = default;

    static std::unique_ptr<IGraphicsAPI> createAPI(APIType type, Window* window);
};