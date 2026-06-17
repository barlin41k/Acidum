find_program(GLSLC_COMPILER glslc REQUIRED)

set(SANDBOX_SOURCES
    src/SandboxApp.cpp
)

set(SHADER_SOURCES
    assets/shaders/shader.vert
    assets/shaders/shader.frag
)

set(SPV_TARGETS "")
foreach(SHADER IN LISTS SHADER_SOURCES)
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPV_FILE "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/spirv/${FILE_NAME}.spv")
    set(DEP_FILE "${SPV_FILE}.d")
    set(ABSOLUTE_SHADER "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}")
    
    add_custom_command(
        OUTPUT ${SPV_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/spirv"
        COMMAND ${GLSLC_COMPILER} ${ABSOLUTE_SHADER} -MD -MF ${DEP_FILE} -o ${SPV_FILE}
        DEPENDS ${ABSOLUTE_SHADER}
        DEPFILE ${DEP_FILE}
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
