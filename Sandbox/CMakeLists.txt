find_program(GLSLC_COMPILER glslc REQUIRED)

file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS "assets/shaders/*.vert" "assets/shaders/*.frag")

set(SPV_TARGETS "")
foreach(SHADER IN LISTS SHADER_SOURCES)
    get_filename_component(FILE_NAME ${SHADER} NAME_WE)
    set(SPV_FILE "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/spirv/${FILE_NAME}.spv")
    
    add_custom_command(
        OUTPUT ${SPV_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/spirv"
        COMMAND ${GLSLC_COMPILER} ${SHADER} -o ${SPV_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling Vulkan shader: ${FILE_NAME}"
    )
    list(APPEND SPV_TARGETS ${SPV_FILE})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPV_TARGETS})

add_executable(Sandbox ${SANDBOX_SOURCES})
add_dependencies(Sandbox CompileShaders)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Sandbox>/assets"
    COMMENT "Copying assets to build directory..."
)

target_link_libraries(Sandbox PRIVATE AcidumCore)
target_include_directories(Sandbox PUBLIC include)
