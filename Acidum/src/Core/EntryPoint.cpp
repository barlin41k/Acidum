#include <chrono>

#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Application.hpp"
#include "Acidum/Core/Platform/SystemInfo.hpp"

namespace Acidum {
    extern std::unique_ptr<Application> CreateApplication();
}

int main() {
    auto startTime = std::chrono::high_resolution_clock::now();

    Acidum::Logger::Init();

    std::string version = std::format("v{}.{}.{}",
        Acidum::Consts::ENGINE_VERSION.major,
        Acidum::Consts::ENGINE_VERSION.minor,
        Acidum::Consts::ENGINE_VERSION.patch);

    ENGINE_INFO("============================================");
    ENGINE_INFO("           Acidum Engine {}", version);
    ENGINE_INFO("============================================");
    ENGINE_INFO("OS         : {}", Acidum::SystemInfo::GetOS());
    ENGINE_INFO("Compiler   : {}", Acidum::SystemInfo::GetCompiler());
    ENGINE_INFO("Arch       : {}", Acidum::SystemInfo::GetArchitecture());
    ENGINE_INFO("Build Type : {}", Acidum::SystemInfo::GetBuildType());
    ENGINE_INFO("============================================");

    {
        try {
            auto app = Acidum::CreateApplication();
            app->run();
        } catch (const std::exception& error) {
            Acidum::Logger::PrintCrashBanner("Unhandled Exception", error.what());
            return EXIT_FAILURE;
        } 
        catch (...) {
            Acidum::Logger::PrintCrashBanner("Unhandled Exception", "Unknown unhandled exception!");
            return EXIT_FAILURE;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = endTime - startTime;

    ENGINE_INFO("Acidum Engine shutdown successfully! Total runtime: {:.2f}s", elapsed.count());
    return EXIT_SUCCESS;
}