#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanInstance.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/Window.hpp"

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

    
    InstanceConfig instanceConfig;
    instanceConfig.appName = m_window->getTitle();
    instanceConfig.appVersion = version;
    instanceConfig.windowExtensions = windowExtensions;

    DeviceConfig deviceConfig;


    m_instance = std::make_unique<VulkanInstance>(instanceConfig);

    m_surface = std::make_unique<VulkanSurface>(*m_instance, m_window);

    m_device = std::make_unique<VulkanDevice>(*m_instance, *m_surface, deviceConfig);

    m_renderer = std::make_unique<VulkanRenderer>(*m_device, *m_surface, m_window);

    ENGINE_INFO("Vulkan initialized!");
}

void VulkanGraphicsAPI::waitIdle() const {
    if (m_device) vkDeviceWaitIdle(m_device->getLogicalDevice());
}

void VulkanGraphicsAPI::setProjectionMatrix(const glm::mat4& proj) {
    static const glm::mat4 clipCorrection(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
    );

    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    m_renderer->setProjectionMatrix(clipCorrection * proj);
}

void VulkanGraphicsAPI::setViewMatrix(const glm::mat4& view) {
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    m_renderer->setViewMatrix(view);
}

std::unique_ptr<IMesh> VulkanGraphicsAPI::createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    return std::make_unique<VulkanMesh>(*m_device, m_renderer->getCommandPool(), vertices, indices);
}

void VulkanGraphicsAPI::drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) {
    if (mesh == nullptr) return;
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");

    if (auto* vulkanMesh = dynamic_cast<VulkanMesh*>(mesh))
        m_renderer->submitMesh(vulkanMesh, modelMatrix);
    else
        ENGINE_WARN("drawMesh warning: trying to draw a non-Vulkan mesh in VulkanGraphicsAPI class!");
}

void VulkanGraphicsAPI::renderFrame() {
    ENGINE_VERIFY(m_renderer != nullptr, "Vulkan Renderer is not initialized!");
    m_renderer->drawFrame();
}

} // namespace Acidum