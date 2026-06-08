#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "Acidum/Core/Base/Types.hpp"
#include "Acidum/Graphics/Interfaces/IGraphicsAPI.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
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

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanPipeline> m_pipeline;
    std::unique_ptr<VulkanSwapChain> m_swapChain;
    std::unique_ptr<VulkanCommandBufferManager> m_commandBufferManager;
    std::unique_ptr<VulkanSyncManager> m_syncManager;
    std::vector<RenderCommand> m_renderQueue;
    std::unique_ptr<VulkanDescriptorManager> m_descriptorManager;

    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif
    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    uint32_t m_currentFrame = 0;
    bool m_framebufferResized = false;

    void recreateSwapChain();
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    // create device
    // create swapchain
    // create pipeline
    // create framebuffers
    // create command pool
    // create buffers vertex and index
    // create command buffer
    // create sync objects
    // create uniform buffers

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator);
};
} // namespace Acidum