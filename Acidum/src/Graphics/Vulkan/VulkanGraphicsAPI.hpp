#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Graphics/Interfaces/IGraphicsAPI.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanInstance.hpp"
#include "Graphics/Vulkan/VulkanSurface.hpp"
#include "Graphics/Vulkan/VulkanSwapChain.hpp"
#include "Graphics/Vulkan/VulkanBuffer.hpp"
#include "Graphics/Vulkan/VulkanPipeline.hpp"
#include "Graphics/Vulkan/VulkanMesh.hpp"
#include "Graphics/Vulkan/VulkanCommandBufferManager.hpp"
#include "Graphics/Vulkan/VulkanSyncManager.hpp"
#include "Graphics/Vulkan/VulkanDescriptorManager.hpp"

namespace Acidum {
class Window; // forward-declaration

class VulkanGraphicsAPI : public IGraphicsAPI {
public:
    VulkanGraphicsAPI(Window* window);
    ~VulkanGraphicsAPI() override;

    void initialize() override;

    std::unique_ptr<IMesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) override;
    void drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) override;

    void setViewMatrix(const glm::mat4& view) override { m_viewMatrix = view; }
    void setProjectionMatrix(const glm::mat4& proj) override;

    void renderFrame() override;

    void waitIdle() const override;
private:
    struct RenderCommand {
        VulkanMesh* mesh;
        glm::mat4 modelMatrix;
    };

    Window* m_window = nullptr;

    std::unique_ptr<VulkanInstance> m_instance;
    std::unique_ptr<VulkanSurface> m_surface;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanPipeline> m_pipeline;
    std::unique_ptr<VulkanSwapChain> m_swapChain;
    std::unique_ptr<VulkanCommandBufferManager> m_commandBufferManager;
    std::unique_ptr<VulkanSyncManager> m_syncManager;
    std::vector<RenderCommand> m_renderQueue;
    std::unique_ptr<VulkanDescriptorManager> m_descriptorManager;

    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

    uint32_t m_currentFrame = 0;
    bool m_framebufferResized = false;

    void recreateSwapChain();
    // create debug messenger
    // create surface
    // create device
    // create swapchain
    // create pipeline
    // create framebuffers
    // create command pool
    // create buffers vertex and index
    // create command buffer
    // create sync objects
    // create uniform buffers

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
};
} // namespace Acidum