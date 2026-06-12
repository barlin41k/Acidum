file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(AcidumCore ${CORE_SOURCES})

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
