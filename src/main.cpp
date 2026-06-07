#include <vulkan/vulkan.h>

#include <cstdlib>

#include "Core/Consts.hpp"
#include "Core/Logger.hpp"
#include "Core/Application.hpp"

int main() {
    Logger::Init();

    ENGINE_INFO("Engine have been started!");

    Application app (Consts::DEFAULT_API);

    try {
        app.run();
    } catch (const std::exception& error) {
        ENGINE_FATAL("Unhandled exception! Reason: {}", error.what());
        return EXIT_FAILURE;
    }

    ENGINE_INFO("Engine shutdown successfully!");
    return EXIT_SUCCESS;
}