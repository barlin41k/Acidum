#pragma once

#include <memory>
#include <vector>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Graphics/Interfaces/IGraphicsAPI.hpp"
#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {

// forward-declaration
class Window;
class VulkanInstance;
class VulkanSurface;
class VulkanDevice;
class VulkanRenderer;
class VulkanStagingManager;

class VulkanGraphicsAPI : public IGraphicsAPI {
public:
    VulkanGraphicsAPI(Window* window);
    ~VulkanGraphicsAPI() override;

    void initialize() override;

    std::shared_ptr<ITexture2D> createTexture2D(const void* data, uint32_t width, uint32_t height) override;

    void beginUpload() override;
    std::unique_ptr<IMesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) override;
    void endUploadAndWait() override;

    void drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) override;

    void setViewMatrix(const glm::mat4& view) override;
    void setProjectionMatrix(const glm::mat4& proj) override;
    void setLightDirection(const glm::vec3& dir) override;
    void setCameraPosition(const glm::vec3& pos) override;

    void renderFrame() override;

    void waitIdle() const override;
private:
    Window* m_window = nullptr;
    std::unique_ptr<VulkanInstance> m_instance;
    std::unique_ptr<VulkanSurface> m_surface;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanRenderer> m_renderer;
    std::unique_ptr<VulkanStagingManager> m_stagingManager;

    static constexpr glm::mat4 CLIP_CORRECTION = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
    );
};

} // namespace Acidum