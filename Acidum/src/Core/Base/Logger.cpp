#include "Acidum/Core/Base/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Acidum {

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init() {
    spdlog::set_pattern("%^[%H:%M:%S.%e] [%l] %v%$");
    s_Logger = spdlog::stdout_color_mt("ENGINE");
    
#ifdef ACIDUM_DEBUG_BUILD
    s_Logger->set_level(spdlog::level::trace);
#else
    s_Logger->set_level(spdlog::level::info);
#endif
}

void Logger::PrintCrashBanner(const char* errorType, const std::string& details) {
    auto& logger = GetLogger();
    
    std::string banner = fmt::format(
        "\n"
        "========================================================\n"
        "                 ACIDUM ENGINE CRASHED!                 \n"
        "========================================================\n"
        "Type    : {}\n"
        "Details : {}\n"
        "========================================================",
        errorType, details
    );

    logger->critical(banner);
    logger->flush();
}

} // namespace Acidum