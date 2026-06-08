add_library(AcidumCore
    src/Core/EntryPoint.cpp
    src/Core/Base/Logger.cpp
    src/Core/Application.cpp
    src/Core/Platform/Window.cpp
    src/Graphics/Vulkan/VulkanGraphicsAPI.cpp
    src/Graphics/Vulkan/VulkanInstance.cpp
    src/Graphics/Vulkan/VulkanDevice.cpp
    src/Graphics/Vulkan/VulkanSwapChain.cpp
    src/Graphics/Vulkan/VulkanBuffer.cpp
    src/Graphics/Vulkan/VulkanPipeline.cpp
    src/Graphics/Vulkan/VulkanMesh.cpp
    src/Graphics/Vulkan/VulkanCommandBufferManager.cpp
    src/Graphics/Vulkan/VulkanSyncManager.cpp
    src/Graphics/Vulkan/VulkanDescriptorManager.cpp
    src/Graphics/GraphicsFactory.cpp
)

target_include_directories(AcidumCore
    PUBLIC  include
    PRIVATE src
)

target_link_libraries(AcidumCore
    PUBLIC
        glfw
        glm::glm
        spdlog::spdlog
    PRIVATE
        Vulkan::Vulkan
)

target_compile_options(AcidumCore PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
    -Wnon-virtual-dtor
)
