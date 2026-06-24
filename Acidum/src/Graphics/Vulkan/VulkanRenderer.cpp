#include "Graphics/Vulkan/VulkanRenderer.hpp"

#include <algorithm>

#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/Window.hpp"
#include "Acidum/Core/Resources/ResourceManager.hpp"
#include "Graphics/Vulkan/VulkanConfigs.hpp"
#include "Graphics/Vulkan/VulkanDescriptorManager.hpp"
#include "Graphics/Vulkan/VulkanDevice.hpp"
#include "Graphics/Vulkan/VulkanImage.hpp"
#include "Graphics/Vulkan/VulkanMesh.hpp"
#include "Graphics/Vulkan/VulkanPipeline.hpp"
#include "Graphics/Vulkan/VulkanSwapChain.hpp"
#include "Graphics/Vulkan/VulkanSyncManager.hpp"
#include "Graphics/Vulkan/VulkanTexture2D.hpp"

namespace Acidum {

VulkanRenderer::VulkanRenderer(const VulkanDevice& device, const VulkanSurface& surface, Window* window, const RendererConfig& config)
    : m_device(device),
      m_surface(surface),
      m_window(window),
      m_config(config)
{
    ENGINE_INFO("Initializing Vulkan Renderer...");

    m_swapChain = std::make_unique<VulkanSwapChain>(m_device, m_surface, m_window, m_config.swapChainConfig);
    createDepthResources();

    m_descriptorManager = std::make_unique<VulkanDescriptorManager>(m_device, Consts::MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorSetLayout> layouts = {
        m_descriptorManager->getGlobalDescriptorSetLayout(),
        m_descriptorManager->getMaterialDescriptorSetLayout()
    };

    m_commandBufferManager = std::make_unique<VulkanCommandBufferManager>(m_device, Consts::MAX_FRAMES_IN_FLIGHT);

    uint32_t imageCount = static_cast<uint32_t>(m_swapChain->getImageViews().size());
    m_syncManager = std::make_unique<VulkanSyncManager>(m_device, Consts::MAX_FRAMES_IN_FLIGHT, imageCount);

    ENGINE_INFO("Vulkan Renderer initialized!");
}

VulkanRenderer::~VulkanRenderer() {
    if (m_device.getLogicalDevice() != VK_NULL_HANDLE)
        vkDeviceWaitIdle(m_device.getLogicalDevice());
}

void VulkanRenderer::submitMesh(VulkanMesh* mesh, const glm::mat4& modelMatrix) {
    if (mesh) m_renderQueue.push_back({mesh, modelMatrix});
}

void VulkanRenderer::drawFrame() {
    vkWaitForFences(m_device.getLogicalDevice(), 1, m_syncManager->getInFlightFencePtr(m_currentFrame), VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_device.getLogicalDevice(), m_swapChain->getSwapChain(), UINT64_MAX, m_syncManager->getAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else ENGINE_VERIFY(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image!");

    if (m_syncManager->getImageInFlightFence(imageIndex) != VK_NULL_HANDLE) {
        VkFence fence = m_syncManager->getImageInFlightFence(imageIndex);
        vkWaitForFences(m_device.getLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    }
    
    m_syncManager->setImageInFlightFence(imageIndex, m_syncManager->getInFlightFence(m_currentFrame));

    vkResetFences(m_device.getLogicalDevice(), 1, m_syncManager->getInFlightFencePtr(m_currentFrame));

    vkResetCommandBuffer(m_commandBufferManager->getCommandBuffer(m_currentFrame), 0);
    recordCommandBuffer(m_commandBufferManager->getCommandBuffer(m_currentFrame), imageIndex);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_syncManager->getAvailableSemaphore(m_currentFrame) };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    
    VkCommandBuffer cmdBuffer = m_commandBufferManager->getCommandBuffer(m_currentFrame);
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkSemaphore signalSemaphores[] = { m_syncManager->getFinishedSemaphore(imageIndex) };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    ENGINE_VERIFY(
        vkQueueSubmit(
            m_device.getGraphicsQueue(), 1, &submitInfo, m_syncManager->getInFlightFence(m_currentFrame)
        ) == VK_SUCCESS,
        "Failed to submit draw command buffer!"
    );

    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapChain->getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(m_device.getPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
        m_framebufferResized = false;
        recreateSwapChain();
    } else ENGINE_VERIFY(result == VK_SUCCESS, "Failed to present swap chain image!");

    m_currentFrame = (m_currentFrame + 1) % Consts::MAX_FRAMES_IN_FLIGHT;
    
    m_renderQueue.clear();
}

void VulkanRenderer::recreateSwapChain() {
    int width = 0, height = 0;
    m_window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0) {
        m_window->getFramebufferSize(&width, &height);
        m_window->waitEvents();
    }

    vkDeviceWaitIdle(m_device.getLogicalDevice());

    m_swapChain->recreate();
    createDepthResources();
    
    uint32_t imageCount = static_cast<uint32_t>(m_swapChain->getImageViews().size());
    m_syncManager->resizeImagesInFlight(imageCount);
}

void VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    ENGINE_VERIFY(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

    VkImageMemoryBarrier barriers[2] {};

    barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barriers[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].image = m_swapChain->getImages()[imageIndex];
    barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barriers[0].subresourceRange.baseMipLevel = 0;
    barriers[0].subresourceRange.levelCount = 1;
    barriers[0].subresourceRange.baseArrayLayer = 0;
    barriers[0].subresourceRange.layerCount = 1;
    barriers[0].srcAccessMask = 0;
    barriers[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barriers[1].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[1].image = m_depthImage->getImage();
    barriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    barriers[1].subresourceRange.baseMipLevel = 0;
    barriers[1].subresourceRange.levelCount = 1;
    barriers[1].subresourceRange.baseArrayLayer = 0;
    barriers[1].subresourceRange.layerCount = 1;
    barriers[1].srcAccessMask = 0;
    barriers[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, 0,
        nullptr, 0,
        nullptr, 2,
        barriers
    );

    VkRenderingAttachmentInfo colorAttachment {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_swapChain->getImageViews()[imageIndex];
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{ m_config.clearColor.r, m_config.clearColor.g, m_config.clearColor.b, m_config.clearColor.a }};

    VkRenderingAttachmentInfo depthAttachment {};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.imageView = m_depthImage->getImageView();
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.clearValue.depthStencil = { 1.0f, 0 };
    
    VkRenderingInfo renderInfo {};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderInfo.renderArea.offset = { 0, 0 };
    renderInfo.renderArea.extent = m_swapChain->getExtent();
    renderInfo.layerCount = 1;
    renderInfo.colorAttachmentCount = 1;
    renderInfo.pColorAttachments = &colorAttachment;
    renderInfo.pDepthAttachment = &depthAttachment;

    vkCmdBeginRendering(commandBuffer, &renderInfo);

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(m_swapChain->getExtent().height);
    viewport.width = static_cast<float>(m_swapChain->getExtent().width);
    viewport.height = -static_cast<float>(m_swapChain->getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapChain->getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    UniformBufferObject ubo {};
    ubo.view = m_viewMatrix;
    ubo.proj = m_projectionMatrix;
    ubo.lightDir = m_lightDirection;
    updateUniformBuffer(m_currentFrame, ubo);

    std::sort(m_renderQueue.begin(), m_renderQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
        auto matA = a.mesh ? a.mesh->getMaterial() : nullptr;
        auto matB = b.mesh ? b.mesh->getMaterial() : nullptr;

        if (!matA || !matB) return matA < matB;

        bool blendA = matA->enableBlending;
        bool blendB = matB->enableBlending;

        if (blendA != blendB) return blendA < blendB;
        
        return matA < matB;
    });

    VulkanPipeline* currentPipeline = nullptr;
    Material* currentMaterial = nullptr;

    VkDescriptorSet globalSet = m_descriptorManager->getGlobalDescriptorSet(m_currentFrame);

    for (const auto& command : m_renderQueue) {
        if (command.mesh != nullptr) {
            auto material = command.mesh->getMaterial();
            VulkanPipeline* pipeline = getOrCreatePipeline(material);

            if (pipeline != currentPipeline) {
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

                vkCmdBindDescriptorSets(
                    commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline->getLayout(), 0, 1, &globalSet, 0, nullptr
                );
                currentPipeline = pipeline;
            }

            if (material != currentMaterial) {
                VkDescriptorSet matSet = m_descriptorManager->getOrCreateMaterialDescriptor(material);
                if (matSet != VK_NULL_HANDLE)
                    vkCmdBindDescriptorSets(
                        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipeline->getLayout(), 1, 1, &matSet, 0, nullptr
                    );
                currentMaterial = material;
            }

            PushContants pc {};
            pc.model = command.modelMatrix;
            pc.baseColor = material->baseColor;

            vkCmdPushConstants(
                commandBuffer, 
                pipeline->getLayout(), 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                0, 
                sizeof(PushContants), 
                &pc
            );

            command.mesh->bind(commandBuffer);
            command.mesh->draw(commandBuffer);
        }
    }

    vkCmdEndRendering(commandBuffer);

    barriers[0].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barriers[0].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barriers[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barriers[0].dstAccessMask = 0;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0, 0,
        nullptr, 0,
        nullptr, 1,
        &barriers[0]
    );

    ENGINE_VERIFY(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
}

void VulkanRenderer::createDepthResources() {
    m_depthFormat = m_device.findDepthFormat();
    VkExtent2D swapChainExtent = m_swapChain->getExtent();

    m_depthImage = std::make_unique<VulkanImage>(
        m_device,
        swapChainExtent.width,
        swapChainExtent.height,
        m_depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
}

void VulkanRenderer::updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo) {
    m_descriptorManager->updateUniformBuffer(currentFrame, ubo);
}

VulkanPipeline* VulkanRenderer::getOrCreatePipeline(Material* material) {
    std::string pipelineKey = material->vertShaderPath + "|" + material->fragShaderPath + "|" + std::to_string(material->enableBlending);

    if (m_pipelineCache.find(pipelineKey) != m_pipelineCache.end())
        return m_pipelineCache[pipelineKey].get();

    auto vertShaderCode = ResourceManager::loadBinaryFile(material->vertShaderPath);
    auto fragShaderCode = ResourceManager::loadBinaryFile(material->fragShaderPath);

    PipelineConfig config = m_config.pipelineConfig;
    config.vertexShaderBytecode = vertShaderCode;
    config.fragmentShaderBytecode = fragShaderCode;
    config.enableAlphaBlending = material->enableBlending;
    config.enableDepthWrite = material->depthWrite;
    config.colorFormat = m_swapChain->getFormat();
    config.depthFormat = m_depthFormat;

    std::vector<VkDescriptorSetLayout> layouts = {
        m_descriptorManager->getGlobalDescriptorSetLayout(),
        m_descriptorManager->getMaterialDescriptorSetLayout()
    };
    
    m_pipelineCache[pipelineKey] = std::make_unique<VulkanPipeline>(m_device, config, layouts);

    return m_pipelineCache[pipelineKey].get();
}

} // namespace Acidum