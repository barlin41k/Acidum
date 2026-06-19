set(CORE_SOURCES
    src/Core/Application.cpp
    src/Core/Base/Logger.cpp
    src/Core/EntryPoint.cpp
    src/Core/Platform/GLFWContext.cpp
    src/Core/Platform/Input.cpp
    src/Core/Platform/Window.cpp
    src/Core/Resources/ResourceManager.cpp
    src/Graphics/GraphicsFactory.cpp
    src/Graphics/Vulkan/VulkanBuffer.cpp
    src/Graphics/Vulkan/VulkanCommandBufferManager.cpp
    src/Graphics/Vulkan/VulkanDescriptorManager.cpp
    src/Graphics/Vulkan/VulkanDevice.cpp
    src/Graphics/Vulkan/VulkanGraphicsAPI.cpp
    src/Graphics/Vulkan/VulkanImage.cpp
    src/Graphics/Vulkan/VulkanInstance.cpp
    src/Graphics/Vulkan/VulkanMesh.cpp
    src/Graphics/Vulkan/VulkanPipeline.cpp
    src/Graphics/Vulkan/VulkanRenderPass.cpp
    src/Graphics/Vulkan/VulkanRenderer.cpp
    src/Graphics/Vulkan/VulkanSurface.cpp
    src/Graphics/Vulkan/VulkanSwapChain.cpp
    src/Graphics/Vulkan/VulkanSyncManager.cpp
    src/Scene/Camera.cpp
)

if (WIN32)
    list(APPEND CORE_SOURCES src/Core/Platform/Windows/PlatformUtils.cpp)
elseif (UNIX AND NOT APPLE)
    list(APPEND CORE_SOURCES src/Core/Platform/Linux/PlatformUtils.cpp)
else()
    message(FATAL_ERROR "Unsupported platform for Acidum Engine!")
endif()

add_library(AcidumVMA STATIC src/Graphics/Vulkan/VulkanMemoryAllocator.cpp)
add_library(AcidumCore ${CORE_SOURCES})

target_include_directories(AcidumVMA SYSTEM PUBLIC ${vulkanmemoryallocator_SOURCE_DIR}/include)
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
        AcidumVMA
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
