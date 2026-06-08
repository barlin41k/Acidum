#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include <cstring>
#include <vector>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Platform/Window.hpp"

namespace Acidum {
VulkanGraphicsAPI::VulkanGraphicsAPI(Window* window) 
    : m_window(window) {}

VulkanGraphicsAPI::~VulkanGraphicsAPI() {
    waitIdle();

    m_descriptorManager.reset();
    m_syncManager.reset();
    m_commandBufferManager.reset();
    m_swapChain.reset();
    m_pipeline.reset();
    m_device.reset();

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

void VulkanGraphicsAPI::initialize() {
    ENGINE_INFO("Initializing Vulkan...");

    m_window->setResizeCallback([this](int /*width*/, int /*height*/) {
        this->m_framebufferResized = true;
    });

    createInstance();
    setupDebugMessenger();
    createSurface();

    m_device = std::make_unique<VulkanDevice>(m_instance, m_surface);

    m_swapChain = std::make_unique<VulkanSwapChain>(*m_device, m_surface, m_window);

    m_pipeline = std::make_unique<VulkanPipeline>(*m_device, m_swapChain->getFormat());

    m_swapChain->createFramebuffers(m_pipeline->getRenderPass());

    m_commandBufferManager = std::make_unique<VulkanCommandBufferManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT);

    uint32_t imageCount = static_cast<uint32_t>(m_swapChain->getImageViews().size());
    m_syncManager = std::make_unique<VulkanSyncManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT, imageCount);

    m_descriptorManager = std::make_unique<VulkanDescriptorManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT, m_pipeline->getDescriptorSetLayout());

    ENGINE_INFO("Vulkan initialized!");
}

void VulkanGraphicsAPI::waitIdle() const {
    if (m_device != VK_NULL_HANDLE) vkDeviceWaitIdle(m_device->getLogicalDevice());
}

void VulkanGraphicsAPI::setProjectionMatrix(const glm::mat4& proj) {
    static const glm::mat4 clipCorrection(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
    );

    m_projectionMatrix = clipCorrection * proj; 
}

std::unique_ptr<IMesh> VulkanGraphicsAPI::createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    return std::make_unique<VulkanMesh>(*m_device, m_commandBufferManager->getCommandPool(), vertices, indices);
}

void VulkanGraphicsAPI::drawMesh(IMesh* mesh, const glm::mat4& modelMatrix) {
    if (mesh == nullptr) return;

    if (auto* vulkanMesh = dynamic_cast<VulkanMesh*>(mesh))
        m_renderQueue.push_back({vulkanMesh, modelMatrix});
    else
        ENGINE_WARN("Warning: Trying to draw a non-Vulkan mesh in VulkanGraphicsAPI class!");
}

void VulkanGraphicsAPI::renderFrame() {
    vkWaitForFences(m_device->getLogicalDevice(), 1, m_syncManager->getInFlightFencePtr(m_currentFrame), VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_device->getLogicalDevice(), m_swapChain->getSwapChain(), UINT64_MAX, m_syncManager->getAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else ENGINE_VERIFY(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image!");

    if (m_syncManager->getImageInFlightFence(imageIndex) != VK_NULL_HANDLE) {
        VkFence fence = m_syncManager->getImageInFlightFence(imageIndex);
        vkWaitForFences(m_device->getLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    }
    
    m_syncManager->setImageInFlightFence(imageIndex, m_syncManager->getInFlightFence(m_currentFrame));

    vkResetFences(m_device->getLogicalDevice(), 1, m_syncManager->getInFlightFencePtr(m_currentFrame));

    vkResetCommandBuffer(m_commandBufferManager->getCommandBuffer(m_currentFrame), 0);
    recordCommandBuffer(m_commandBufferManager->getCommandBuffer(m_currentFrame), imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_syncManager->getAvailableSemaphore(m_currentFrame)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    
    VkCommandBuffer cmdBuffer = m_commandBufferManager->getCommandBuffer(m_currentFrame);
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkSemaphore signalSemaphores[] = {m_syncManager->getFinishedSemaphore(imageIndex)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    ENGINE_VERIFY(vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, m_syncManager->getInFlightFence(m_currentFrame)) == VK_SUCCESS, "Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(m_device->getPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
        m_framebufferResized = false;
        recreateSwapChain();
    } else ENGINE_VERIFY(result == VK_SUCCESS, "Failed to present swap chain image!");

    m_currentFrame = (m_currentFrame + 1) % Consts::MAX_FRAMES_IN_FLIGHT;
    
    m_renderQueue.clear();
}

void VulkanGraphicsAPI::recreateSwapChain() {
    ENGINE_DEBUG("Recreating Swap Chain...");

    int width = 0, height = 0;
    m_window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0) {
        m_window->getFramebufferSize(&width, &height);
        m_window->waitEvents();
    }

    vkDeviceWaitIdle(m_device->getLogicalDevice());

    m_swapChain->recreate();
    
    m_pipeline.reset();
    m_pipeline = std::make_unique<VulkanPipeline>(*m_device, m_swapChain->getFormat());
    
    m_swapChain->createFramebuffers(m_pipeline->getRenderPass());
    
    uint32_t imageCount = static_cast<uint32_t>(m_swapChain->getImageViews().size());
    m_syncManager.reset();
    m_syncManager = std::make_unique<VulkanSyncManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT, imageCount);
    
    m_descriptorManager.reset();
    m_descriptorManager = std::make_unique<VulkanDescriptorManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT, m_pipeline->getDescriptorSetLayout());

    ENGINE_DEBUG("Swap Chain recreated!");
}

VkBool32 VulkanGraphicsAPI::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /*pUserData*/) {

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        ENGINE_ERROR("Vulkan Validation: {}", pCallbackData->pMessage);
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        ENGINE_WARN("Vulkan Validation: {}", pCallbackData->pMessage);
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        ENGINE_DEBUG("Vulkan Validation: {}", pCallbackData->pMessage);
    else
        ENGINE_TRACE("Vulkan Validation: {}", pCallbackData->pMessage); 

    return VK_FALSE;
}

VkResult VulkanGraphicsAPI::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanGraphicsAPI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, debugMessenger, pAllocator);
}

bool VulkanGraphicsAPI::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

void VulkanGraphicsAPI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

std::vector<const char*> VulkanGraphicsAPI::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

void VulkanGraphicsAPI::createInstance() {
    ENGINE_VERIFY(!m_enableValidationLayers || checkValidationLayerSupport(), "Validation layers requested, but not available!");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    
    appInfo.pApplicationName = m_window->getTitle().c_str();

    auto appVersion = m_window->getVersion(); 
    appInfo.applicationVersion = VK_MAKE_VERSION(
        appVersion.major,
        appVersion.minor,
        appVersion.patch
    );

    appInfo.pEngineName = Consts::ENGINE_NAME.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(
        Consts::ENGINE_VERSION.major, 
        Consts::ENGINE_VERSION.minor,
        Consts::ENGINE_VERSION.patch
    );

    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (m_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    ENGINE_VERIFY(vkCreateInstance(&createInfo, nullptr, &m_instance) == VK_SUCCESS, "Failed to create instance!");
    ENGINE_INFO("Vulkan instance created!");
}

void VulkanGraphicsAPI::setupDebugMessenger() {
    if (!m_enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    ENGINE_VERIFY(CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
}

void VulkanGraphicsAPI::createSurface() {
    ENGINE_VERIFY(glfwCreateWindowSurface(m_instance, m_window->getWindow(), nullptr, &m_surface) == VK_SUCCESS, "Failed to create window surface!");
    ENGINE_INFO("Window surface created!");
}

void VulkanGraphicsAPI::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    ENGINE_VERIFY(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_pipeline->getRenderPass();
    renderPassInfo.framebuffer = m_swapChain->getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChain->getExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(m_swapChain->getExtent().height); // starting from bottom
    viewport.width = static_cast<float>(m_swapChain->getExtent().width);
    viewport.height = -static_cast<float>(m_swapChain->getExtent().height); // GLM y-axis from OpenGL bug fix
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChain->getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDescriptorSet currentDescriptorSet = m_descriptorManager->getDescriptorSet(m_currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getLayout(), 0, 1, &currentDescriptorSet, 0, nullptr);

    UniformBufferObject ubo {};
    ubo.view = m_viewMatrix;
    ubo.proj = m_projectionMatrix;
    updateUniformBuffer(m_currentFrame, ubo);

    for (const auto& command : m_renderQueue) {
        if (command.mesh != nullptr) {
            vkCmdPushConstants(
                commandBuffer, 
                m_pipeline->getLayout(), 
                VK_SHADER_STAGE_VERTEX_BIT, 
                0, 
                sizeof(glm::mat4), 
                &command.modelMatrix
            );

            command.mesh->bind(commandBuffer);
            command.mesh->draw(commandBuffer);
        }
    }

    vkCmdEndRenderPass(commandBuffer);

    ENGINE_VERIFY(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
}

void VulkanGraphicsAPI::updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo) {
    m_descriptorManager->updateUniformBuffer(currentFrame, ubo);
}
} // namespace Acidum