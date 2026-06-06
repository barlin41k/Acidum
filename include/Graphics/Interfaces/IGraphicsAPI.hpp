#pragma once

#include <memory>

#include "Core/Types.hpp"
#include "Graphics/Interfaces/IMesh.hpp"

class IGraphicsAPI {
public:
    virtual ~IGraphicsAPI() = default;

    virtual void initialize() = 0;
    
    virtual std::unique_ptr<IMesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) = 0;
    virtual void drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) = 0;

    virtual void setViewMatrix(const glm::mat4& view) = 0;
    virtual void setProjectionMatrix(const glm::mat4& proj) = 0;

    virtual void renderFrame() = 0;

    virtual void waitIdle() const = 0;
};