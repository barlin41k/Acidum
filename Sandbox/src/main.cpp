#include <cstdlib>

#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Base/Logger.hpp"
#include "SandboxApp.hpp"

int main() {
    Acidum::Logger::Init();

    ENGINE_INFO("Engine have been started!");

    SandboxApp app (Acidum::Consts::ENGINE_DEFAULT_API);

    try {
        app.run();
    } catch (const std::exception& error) {
        ENGINE_FATAL("Unhandled exception! Reason: {}", error.what());
        return EXIT_FAILURE;
    }

    ENGINE_INFO("Engine shutdown successfully!");
    return EXIT_SUCCESS;
}