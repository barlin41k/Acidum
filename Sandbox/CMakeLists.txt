add_executable(Sandbox
    src/SandboxApp.cpp
)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Sandbox>/assets"
    COMMENT "Copying shaders to build directory..."
)

target_link_libraries(Sandbox PRIVATE AcidumCore)
target_include_directories(Sandbox PUBLIC include)