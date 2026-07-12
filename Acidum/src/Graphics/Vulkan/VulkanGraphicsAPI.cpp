#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include "Acidum/Core/Platform/Window.hpp"
#include "Graphics/Vulkan/VulkanLogger.hpp"
#include "Graphics/Vulkan/VulkanConfigs.hpp"
#include "Graphics/Vulkan/VulkanSurface.hpp"
#include "Graphics/Vulkan/VulkanInstance.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanMesh.hpp"
#include "Graphics/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Vulkan/VulkanStagingManager.hpp"
#include "Graphics/Vulkan/VulkanTexture2D.hpp"

namespace Acidum {

VulkanGraphicsAPI::VulkanGraphicsAPI(Window* window) 
    : m_window(window) {}

VulkanGraphicsAPI::~VulkanGraphicsAPI() {
    waitIdle();
}

void VulkanGraphicsAPI::initialize(const GraphicsConfig& config) {
    VK_INFO("Initializing Vulkan...");

    m_window->setResizeCallback([this](int /*width*/, int /*height*/) {
        if (m_renderer) m_renderer->setFramebufferResized(true);
    });


    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> windowExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    auto appVersion = m_window->getVersion(); 
    uint32_t version = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);

    const auto& defaultVertexLayout = VertexLayout::GetDefaultPBRLayout();
    auto bindingDescription = VulkanMesh::getBindingDescription(defaultVertexLayout);
    auto attributeDescription = VulkanMesh::getAttributeDescriptions(defaultVertexLayout);

    
    InstanceConfig instanceConfig;
    instanceConfig.appName = m_window->getTitle();
    instanceConfig.appVersion = version;
    instanceConfig.windowExtensions = windowExtensions;

    DeviceConfig deviceConfig;
    deviceConfig.requiredFeatures.fillModeNonSolid = config.supportWireframe ? VK_TRUE : VK_FALSE;
    deviceConfig.gpuPreference = config.gpuPreference;
    deviceConfig.preferredDeviceName = config.preferredDeviceName;

    SwapChainConfig swapChainConfig;
    swapChainConfig.preferredPresentMode = config.enableVSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;

    PipelineConfig pipelineConfig;
    pipelineConfig.vertexBindingDescriptions = { bindingDescription };
    pipelineConfig.vertexAttributeDescriptions = { attributeDescription };

    RendererConfig rendererConfig;
    rendererConfig.swapChainConfig = swapChainConfig;
    rendererConfig.pipelineConfig = pipelineConfig;
    rendererConfig.clearColor = config.clearColor;
    

    m_instance = std::make_unique<VulkanInstance>(instanceConfig);

    m_surface = std::make_unique<VulkanSurface>(*m_instance, m_window);

    m_device = std::make_unique<VulkanDevice>(*m_instance, *m_surface, deviceConfig);

    m_stagingManager = std::make_unique<VulkanStagingManager>(*m_device);

    m_renderer = std::make_unique<VulkanRenderer>(*m_device, *m_surface, m_window, rendererConfig);

    VK_INFO("Vulkan initialized!");
}

std::vector<GPUAdapterInfo> VulkanGraphicsAPI::enumerateAvailableAdapters() {
    VkInstance instance = m_instance->getInstance();

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::vector<GPUAdapterInfo> result;
    result.reserve(deviceCount);

    for (uint32_t i = 0; i < deviceCount; ++i) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(devices[i], &memoryProperties);

        VkDeviceSize vramSize = 0;
        for (uint32_t j = 0; j < memoryProperties.memoryHeapCount; j++) {
            if (memoryProperties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                vramSize = std::max(vramSize, memoryProperties.memoryHeaps[j].size);
        }

        result.push_back({
            .id = i,
            .name = properties.deviceName,
            .vramSizeMB = static_cast<uint64_t>(vramSize / (1024 * 1024)),
            .isDiscrete = (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        });
    }

    return result;
}

void VulkanGraphicsAPI::waitIdle() const {
    VkDevice device = m_device->getLogicalDevice();
    if (device != VK_NULL_HANDLE) vkDeviceWaitIdle(device);
}

void VulkanGraphicsAPI::setProjectionMatrix(const glm::mat4& proj) {
    ACIDUM_ASSERT(m_renderer != nullptr, "Renderer is not initialized!");
    m_renderer->setProjectionMatrix(CLIP_CORRECTION * proj);
}

void VulkanGraphicsAPI::setViewMatrix(const glm::mat4& view) {
    ACIDUM_ASSERT(m_renderer != nullptr, "Renderer is not initialized!");
    m_renderer->setViewMatrix(view);
}

void VulkanGraphicsAPI::setLightDirection(const glm::vec3& dir) {
    ACIDUM_ASSERT(m_renderer != nullptr, "Renderer is not initialized!");
    m_renderer->setLightDirection(dir);
}

void VulkanGraphicsAPI::setCameraPosition(const glm::vec3& pos) {
    m_renderer->setCameraPosition(pos);
}

std::shared_ptr<ITexture2D> VulkanGraphicsAPI::createTexture2D(const void* data, uint32_t width, uint32_t height, bool isSRGB) {
    return std::make_shared<VulkanTexture2D>(*m_device, m_stagingManager.get(), data, width, height, isSRGB);
}

void VulkanGraphicsAPI::beginUpload() {
    m_stagingManager->begin();
}

std::unique_ptr<IMesh> VulkanGraphicsAPI::createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const VertexLayout& layout) {
    ACIDUM_ASSERT(m_renderer != nullptr, "Renderer is not initialized!");
    return std::make_unique<VulkanMesh>(*m_device, m_stagingManager.get(), vertices, indices, layout);
}

void VulkanGraphicsAPI::endUploadAndWait() {
    m_stagingManager->submit();
    m_stagingManager->waitForUpload();
}

void VulkanGraphicsAPI::drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) {
    ACIDUM_ASSERT(m_renderer != nullptr, "Renderer is not initialized!");
    if (mesh == nullptr) {
        VK_WARN("Mesh can not be empty!");
        return;
    }

    if (auto* vulkanMesh = dynamic_cast<VulkanMesh*>(mesh))
        m_renderer->submitMesh(vulkanMesh, modelMatrix);
    else
        VK_WARN("Trying to draw non-Vulkan mesh (type: {}) in drawMesh!", typeid(*mesh).name()); 
}

void VulkanGraphicsAPI::renderFrame() {
    ACIDUM_ASSERT(m_renderer != nullptr, "Renderer is not initialized!");
    m_renderer->drawFrame();
}

} // namespace Acidum