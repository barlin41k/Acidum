add_executable(Sandbox
    src/main.cpp
    src/SandboxApp.cpp
)

target_link_libraries(Sandbox PRIVATE AcidumCore)
target_include_directories(Sandbox PUBLIC include)