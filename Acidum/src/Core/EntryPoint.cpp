
#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Application.hpp"

namespace Acidum {
    extern Application* CreateApplication();
}

int main() {
    Acidum::Logger::Init();

    ENGINE_INFO("Acidum Engine has started!");

    auto app = Acidum::CreateApplication();

    try {
        app->run();
    } catch (const std::exception& error) {
        ENGINE_FATAL("An unhandled exception occurred in Acidum Engine! Reason: {}", error.what());

        delete app;
        return EXIT_FAILURE;
    }

    delete app;
    ENGINE_INFO("Acidum Engine shutdown successfully!");
    return EXIT_SUCCESS;
}