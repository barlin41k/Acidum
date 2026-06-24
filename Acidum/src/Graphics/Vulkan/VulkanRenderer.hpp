#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <memory>
#include <vector>

#include "Acidum/Graphics/Material.hpp"
#include "Graphics/Vulkan/VulkanTypes.hpp"
#include "Graphics/Vulkan/VulkanConfigs.hpp"
#include "Graphics/Vulkan/VulkanCommandBufferManager.hpp"

namespace Acidum {

// forward-declaration
class Window;
class VulkanImage;
class VulkanMesh;
class VulkanSurface;
class VulkanDevice;
class VulkanSwapChain;
class VulkanPipeline;
class VulkanSyncManager;
class VulkanDescriptorManager;

struct RenderCommand {
    VulkanMesh* mesh;
    glm::mat4 modelMatrix;
};

class VulkanRenderer {
public:
    VulkanRenderer(const VulkanDevice& device, const VulkanSurface& surface, Window* window, const RendererConfig& config);
    ~VulkanRenderer();

    VulkanRenderer(const VulkanRenderer&) = delete;
    VulkanRenderer& operator=(const VulkanRenderer&) = delete;

    void drawFrame();
    void submitMesh(VulkanMesh* mesh, const glm::mat4& modelMatrix);

    void setViewMatrix(const glm::mat4& view) { m_viewMatrix = view; }
    void setProjectionMatrix(const glm::mat4& proj) { m_projectionMatrix = proj; }
    void setLightDirection(const glm::vec3& direction) { m_lightDirection = direction; }
    void setFramebufferResized(bool resized) { m_framebufferResized = resized; }

    VkCommandPool getCommandPool() const { return m_commandBufferManager->getCommandPool(); }
private:
    const VulkanDevice& m_device;
    const VulkanSurface& m_surface;
    Window* m_window;

    RendererConfig m_config;

    std::unique_ptr<VulkanSwapChain> m_swapChain;
    std::unordered_map<std::string, std::unique_ptr<VulkanPipeline>> m_pipelineCache;
    std::unique_ptr<VulkanCommandBufferManager> m_commandBufferManager;
    std::unique_ptr<VulkanSyncManager> m_syncManager;
    std::unique_ptr<VulkanDescriptorManager> m_descriptorManager;
    std::unique_ptr<VulkanImage> m_depthImage;
    VkFormat m_depthFormat;

    std::vector<RenderCommand> m_renderQueue;
    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
    glm::vec3 m_lightDirection = glm::vec3(1.0f, 1.0f, 1.0f);

    uint32_t m_currentFrame = 0;
    bool m_framebufferResized = false;

    void createDepthResources();
    void recreateSwapChain();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
    VulkanPipeline* getOrCreatePipeline(Material* material);
};

} // namespace Acidum