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
src/Window.cpp
)

target_link_libraries(${PROJECT_NAME}
    Vulkan::Vulkan
    glfw
    glm::glm
)

target_include_directories(${PROJECT_NAME} PRIVATE include/)