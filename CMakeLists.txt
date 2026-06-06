cmake_minimum_required(VERSION 3.20)
project(VulkanTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME}
src/main.cpp
src/Core/Application.cpp
src/Core/Window.cpp
src/Graphics/Vulkan/VulkanGraphicsAPI.cpp
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

target_link_libraries(${PROJECT_NAME}
    Vulkan::Vulkan
    glfw
    glm::glm
)

target_include_directories(${PROJECT_NAME} PRIVATE include/)