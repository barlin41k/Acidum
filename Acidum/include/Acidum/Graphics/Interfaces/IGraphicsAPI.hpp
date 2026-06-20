#pragma once

#include <memory>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Graphics/Interfaces/IMesh.hpp"
#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {

class IGraphicsAPI {
public:
    virtual ~IGraphicsAPI() = default;

    virtual void initialize() = 0;
    
    virtual std::shared_ptr<ITexture2D> createTexture2D(const void* data, uint32_t width, uint32_t height) = 0;
    
    virtual void beginUpload() = 0;
    virtual std::unique_ptr<IMesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) = 0;
    virtual void endUploadAndWait() = 0;

    virtual void drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) = 0;

    virtual void setViewMatrix(const glm::mat4& view) = 0;
    virtual void setProjectionMatrix(const glm::mat4& proj) = 0;

    virtual void renderFrame() = 0;

    virtual void waitIdle() const = 0;
};

} // namespace Acidum