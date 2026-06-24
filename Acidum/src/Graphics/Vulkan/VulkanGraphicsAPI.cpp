#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/Window.hpp"
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

void VulkanGraphicsAPI::initialize() {
    ENGINE_INFO("Initializing Vulkan...");

    m_window->setResizeCallback([this](int /*width*/, int /*height*/) {
        if (m_renderer) m_renderer->setFramebufferResized(true);
    });


    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> windowExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    auto appVersion = m_window->getVersion(); 
    uint32_t version = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);

    auto bindingDesc = VulkanMesh::getBindingDescription();
    auto attrDesc = VulkanMesh::getAttributeDescriptions();

    
    InstanceConfig instanceConfig;
    instanceConfig.appName = m_window->getTitle();
    instanceConfig.appVersion = version;
    instanceConfig.windowExtensions = windowExtensions;

    DeviceConfig deviceConfig;
    deviceConfig.requiredFeatures.fillModeNonSolid = VK_TRUE;

    SwapChainConfig swapChainConfig;

    PipelineConfig pipelineConfig;
    pipelineConfig.vertexBindingDescriptions = { bindingDesc };
    pipelineConfig.vertexAttributeDescriptions = { attrDesc.begin(), attrDesc.end() };

    RendererConfig rendererConfig;
    rendererConfig.swapChainConfig = swapChainConfig;
    rendererConfig.pipelineConfig = pipelineConfig;
    

    m_instance = std::make_unique<VulkanInstance>(instanceConfig);

    m_surface = std::make_unique<VulkanSurface>(*m_instance, m_window);

    m_device = std::make_unique<VulkanDevice>(*m_instance, *m_surface, deviceConfig);

    m_stagingManager = std::make_unique<VulkanStagingManager>(*m_device);

    m_renderer = std::make_unique<VulkanRenderer>(*m_device, *m_surface, m_window, rendererConfig);

    ENGINE_INFO("Vulkan initialized!");
}

void VulkanGraphicsAPI::waitIdle() const {
    if (m_device) vkDeviceWaitIdle(m_device->getLogicalDevice());
}

void VulkanGraphicsAPI::setProjectionMatrix(const glm::mat4& proj) {
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    m_renderer->setProjectionMatrix(CLIP_CORRECTION * proj);
}

void VulkanGraphicsAPI::setViewMatrix(const glm::mat4& view) {
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    m_renderer->setViewMatrix(view);
}

void VulkanGraphicsAPI::setLightDirection(const glm::vec3& dir) {
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    m_renderer->setLightDirection(dir);
}

void VulkanGraphicsAPI::setCameraPosition(const glm::vec3& pos) {
    m_renderer->setCameraPosition(pos);
}

std::shared_ptr<ITexture2D> VulkanGraphicsAPI::createTexture2D(const void* data, uint32_t width, uint32_t height) {
    return std::make_shared<VulkanTexture2D>(*m_device, m_stagingManager.get(), data, width, height);
}

void VulkanGraphicsAPI::beginUpload() {
    m_stagingManager->begin();
}

std::unique_ptr<IMesh> VulkanGraphicsAPI::createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    return std::make_unique<VulkanMesh>(*m_device, m_stagingManager.get(), vertices, indices);
}

void VulkanGraphicsAPI::endUploadAndWait() {
    m_stagingManager->submit();
    m_stagingManager->waitForUpload();
}

void VulkanGraphicsAPI::drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) {
    if (mesh == nullptr) return;
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");

    if (auto* vulkanMesh = dynamic_cast<VulkanMesh*>(mesh))
        m_renderer->submitMesh(vulkanMesh, modelMatrix);
    else
        ENGINE_WARN("Trying to draw a non-Vulkan mesh in VulkanGraphicsAPI class!");
}

void VulkanGraphicsAPI::renderFrame() {
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    m_renderer->drawFrame();
}

} // namespace Acidum