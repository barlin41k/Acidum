#include <chrono>

#include "Acidum/Core/Base/Consts.hpp"
#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Application.hpp"
#include "Acidum/Core/Platform/SystemInfo.hpp"

namespace Acidum {
    extern std::unique_ptr<Application> CreateApplication();
}

namespace {

using Clock = std::chrono::high_resolution_clock;

void startupLog() {
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
}

void shutdownLog(Clock::time_point startTime, Clock::time_point endTime) {
    std::chrono::duration<float> elapsed = endTime - startTime;

    auto hours = std::chrono::duration_cast<std::chrono::hours>(elapsed);
    elapsed -= hours;

    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(elapsed);
    elapsed -= minutes;

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed - seconds);

    if (hours.count() > 0)
        ENGINE_INFO("Acidum Engine shutdown successfully! Total runtime: {}h {}m {}s", 
                    hours.count(), minutes.count(), seconds.count());
    else if (minutes.count() > 0)
        ENGINE_INFO("Acidum Engine shutdown successfully! Total runtime: {}m {}.{}s", 
                    minutes.count(), seconds.count(), milliseconds.count());
    else
        ENGINE_INFO("Acidum Engine shutdown successfully! Total runtime: {}.{}s", 
                    seconds.count(), milliseconds.count());
}

} // namespace

int main() {
    auto startTime = Clock::now();

    Acidum::Logger::Init();
    startupLog();

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

    auto endTime = Clock::now();
    
    shutdownLog(startTime, endTime);
    return EXIT_SUCCESS;
}