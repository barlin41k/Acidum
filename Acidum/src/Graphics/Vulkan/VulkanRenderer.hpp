#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Graphics/Vulkan/VulkanMesh.hpp"
#include "Graphics/Vulkan/VulkanSurface.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanSwapChain.hpp"
#include "Graphics/Vulkan/VulkanPipeline.hpp"
#include "Graphics/Vulkan/VulkanCommandBufferManager.hpp"
#include "Graphics/Vulkan/VulkanSyncManager.hpp"
#include "Graphics/Vulkan/VulkanDescriptorManager.hpp"

namespace Acidum {
class Window;

struct RenderCommand {
    VulkanMesh* mesh;
    glm::mat4 modelMatrix;
};

class VulkanRenderer {
public:
    VulkanRenderer(VulkanDevice& device, VulkanSurface& surface, Window* window);
    ~VulkanRenderer();

    VulkanRenderer(const VulkanRenderer&) = delete;
    VulkanRenderer& operator=(const VulkanRenderer&) = delete;

    void drawFrame();
    void submitMesh(VulkanMesh* mesh, const glm::mat4& modelMatrix);

    void setViewMatrix(const glm::mat4& view) { m_viewMatrix = view; }
    void setProjectionMatrix(const glm::mat4& proj) { m_projectionMatrix = proj; }
    void setFramebufferResized(bool resized) { m_framebufferResized = resized; }

    VkCommandPool getCommandPool() const { return m_commandBufferManager->getCommandPool(); }
private:
    VulkanDevice& m_device;
    VulkanSurface& m_surface;
    Window* m_window;

    std::unique_ptr<VulkanSwapChain> m_swapChain;
    std::unique_ptr<VulkanPipeline> m_pipeline;
    std::unique_ptr<VulkanCommandBufferManager> m_commandBufferManager;
    std::unique_ptr<VulkanSyncManager> m_syncManager;
    std::unique_ptr<VulkanDescriptorManager> m_descriptorManager;

    std::vector<RenderCommand> m_renderQueue;
    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

    uint32_t m_currentFrame = 0;
    bool m_framebufferResized = false;

    void recreateSwapChain();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
};

} // namespace Acidum