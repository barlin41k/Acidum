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

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.3
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG        release
)
FetchContent_MakeAvailable(tinyobjloader)

set(SPDLOG_INSTALL OFF CACHE INTERNAL "")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

message(STATUS "===========================================")
message(STATUS "Dependencies loaded successfully!")
message(STATUS "===========================================")

find_package(Vulkan REQUIRED)

add_subdirectory(Acidum)
add_subdirectory(Sandbox)