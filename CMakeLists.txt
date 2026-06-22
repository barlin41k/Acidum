cmake_minimum_required(VERSION 3.20)
project(Acidum)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug/Release/RelWithDebInfo)" FORCE)
    message(STATUS "Build type not specified, defaulting to Debug")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(ACIDUM_DEBUG)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

message(STATUS "===========================================")
message(STATUS "Starting to load external libraries by FetchContent...")
message(STATUS "===========================================")

set(GLFW_VERSION 3.4)
set(GLM_VERSION 1.0.3)
set(STB_VERSION 31c1ad37456438565541f4919958214b6e762fb4)
set(TINYOBJLOADER_VERSION 45636bdcef1a4fec140346b90c0b50bf0bc3e23b)
set(SPDLOG_VERSION v1.17.0)
set(VMA_VERSION v3.4.0)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        ${GLFW_VERSION}
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        ${GLM_VERSION}
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        ${STB_VERSION}
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG        ${TINYOBJLOADER_VERSION}
)
FetchContent_MakeAvailable(tinyobjloader)

set(SPDLOG_INSTALL OFF CACHE INTERNAL "")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        ${SPDLOG_VERSION}
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG        ${VMA_VERSION}
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

message(STATUS "===========================================")
message(STATUS "Dependencies loaded successfully!")
message(STATUS "===========================================")

find_package(Vulkan REQUIRED)

add_subdirectory(Acidum)
add_subdirectory(Sandbox)