#pragma once

#include <memory>

#include "Core/Types.hpp"
#include "Graphics/Interfaces/IMesh.hpp"

class IGraphicsAPI {
public:
    virtual ~IGraphicsAPI() = default;

    virtual void initialize() = 0;
    
    virtual std::unique_ptr<IMesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) = 0;
    virtual void drawMesh(IMesh* mesh) = 0;
    virtual void renderFrame() = 0;

    virtual void waitIdle() const = 0;
};