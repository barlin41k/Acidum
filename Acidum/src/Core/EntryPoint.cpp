#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Application.hpp"

namespace Acidum {
    extern std::unique_ptr<Application> CreateApplication();
}

int main() {
    Acidum::Logger::Init();

    ENGINE_INFO("Acidum Engine v{}.{}.{} started!",
        Acidum::Consts::ENGINE_VERSION.major,
        Acidum::Consts::ENGINE_VERSION.minor,
        Acidum::Consts::ENGINE_VERSION.patch
    );

    {
        try {
            auto app = Acidum::CreateApplication();
            app->run();
        } catch (const std::exception& error) {
            ENGINE_FATAL("An unhandled exception occurred in Acidum Engine! Reason: {}", error.what());
            return EXIT_FAILURE;
        }
    }

    ENGINE_INFO("Acidum Engine shutdown successfully!");
    return EXIT_SUCCESS;
}