#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>

#include "Core/Consts.hpp"
#include "Core/Window.hpp"

VulkanGraphicsAPI::VulkanGraphicsAPI(Window* window) 
    : m_window(window) {}

void VulkanGraphicsAPI::initialize() {
    m_window->setResizeCallback([this](int width, int height) {
        this->m_framebufferResized = true;
    });

    createInstance();
    setupDebugMessenger();
    createSurface();

    m_device = std::make_unique<VulkanDevice>(m_instance, m_surface);

    m_swapChain = std::make_unique<VulkanSwapChain>(*m_device, m_surface, m_window);

    m_pipeline = std::make_unique<VulkanPipeline>(*m_device, m_swapChain->getFormat(), m_swapChain->getExtent());

    m_swapChain->createFramebuffers(m_pipeline->getRenderPass());

    m_commandBufferManager = std::make_unique<VulkanCommandBufferManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT);

    uint32_t imageCount = static_cast<uint32_t>(m_swapChain->getImageViews().size());
    m_syncManager = std::make_unique<VulkanSyncManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT, imageCount);

    m_descriptorManager = std::make_unique<VulkanDescriptorManager>(*m_device, Consts::MAX_FRAMES_IN_FLIGHT, m_pipeline->getDescriptorSetLayout());
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
        std::cerr << "Warning: Trying to draw a non-Vulkan mesh in VulkanGraphicsAPI!" << std::endl;
}

void VulkanGraphicsAPI::renderFrame() {
    vkWaitForFences(m_device->getLogicalDevice(), 1, m_syncManager->getInFlightFencePtr(m_currentFrame), VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_device->getLogicalDevice(), m_swapChain->getSwapChain(), UINT64_MAX, m_syncManager->getAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        m_swapChain->recreate(m_pipeline->getRenderPass());
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acquire swap chain image!");

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

    if (vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, m_syncManager->getInFlightFence(m_currentFrame)) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer!");

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
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        m_framebufferResized = false;
        m_swapChain->recreate(m_pipeline->getRenderPass());
    }
    else if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image!");

    m_currentFrame = (m_currentFrame + 1) % Consts::MAX_FRAMES_IN_FLIGHT;
    
    m_renderQueue.clear();
}

VkBool32 VulkanGraphicsAPI::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult VulkanGraphicsAPI::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanGraphicsAPI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) func(instance, debugMessenger, pAllocator);
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

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanGraphicsAPI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
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
    if (m_enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers requested, but not available!");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanTest";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
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

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance!");
}

void VulkanGraphicsAPI::setupDebugMessenger() {
    if (!m_enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("Failed to set up debug messenger!");
}

void VulkanGraphicsAPI::createSurface() {
    if (glfwCreateWindowSurface(m_instance, m_window->getWindow(), nullptr, &m_surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface!");
}

void VulkanGraphicsAPI::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin recording command buffer!");

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

    for (const auto& command : m_renderQueue) {
        if (command.mesh != nullptr) {
            UniformBufferObject ubo{};
            ubo.model = command.modelMatrix;
            ubo.view = m_viewMatrix;
            ubo.proj = m_projectionMatrix;

            updateUniformBuffer(m_currentFrame, ubo);

            command.mesh->bind(commandBuffer);
            command.mesh->draw(commandBuffer);
        }
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer!");
}

void VulkanGraphicsAPI::updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo) {
    m_descriptorManager->updateUniformBuffer(currentFrame, ubo);
}