cmake_minimum_required(VERSION 3.20)
project(VulkanTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug/Release/RelWithDebInfo)" FORCE)
    message(STATUS "Build type not specified, defaulting to Debug")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.17.0
)
set(SPDLOG_INSTALL OFF CACHE INTERNAL "") 
FetchContent_MakeAvailable(spdlog)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Core/Logger.cpp
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

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    spdlog::spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE include/)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
    -Wnon-virtual-dtor
)
