#pragma once

#include <memory>
#include <vector>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Graphics/Interfaces/IGraphicsAPI.hpp"

namespace Acidum {

// forward-declaration
class Window;
class VulkanInstance;
class VulkanDevice;
class VulkanRenderer;
class VulkanSurface;

class VulkanGraphicsAPI : public IGraphicsAPI {
public:
    VulkanGraphicsAPI(Window* window);
    ~VulkanGraphicsAPI() override;

    void initialize() override;

    std::unique_ptr<IMesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) override;
    void drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) override;

    void setViewMatrix(const glm::mat4& view) override;
    void setProjectionMatrix(const glm::mat4& proj) override;

    void renderFrame() override;

    void waitIdle() const override;
private:

    Window* m_window = nullptr;

    std::unique_ptr<VulkanInstance> m_instance;
    std::unique_ptr<VulkanSurface> m_surface;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanRenderer> m_renderer;
};

} // namespace Acidum