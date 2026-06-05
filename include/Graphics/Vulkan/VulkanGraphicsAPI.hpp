#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

#include "Graphics/Interfaces/IGraphicsAPI.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanSwapChain.hpp"
#include "Graphics/Vulkan/VulkanBuffer.hpp"

class Window; // forward-declaration

class VulkanGraphicsAPI : public IGraphicsAPI {
public:
    VulkanGraphicsAPI(Window* window);
    ~VulkanGraphicsAPI() override;

    void initialize() override;
    void renderFrame() override;
    void waitIdle() const override;
private:
    Window* m_window = nullptr;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanSwapChain> m_swapChain;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::unique_ptr<VulkanBuffer> m_vertexBuffer;
    std::unique_ptr<VulkanBuffer> m_indexBuffer;

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

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    // create device
    // create swapchain
    void createRenderPass();
    void createGraphicsPipeline();
    //create framebuffers
    void createCommandPool();
    void createVertexBuffer();
    void createIndexBuffer();
    void createCommandBuffer();
    void createSyncObjects();

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

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