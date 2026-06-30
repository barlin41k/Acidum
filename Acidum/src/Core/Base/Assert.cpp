#include "Acidum/Core/Base/Assert.hpp"

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

void PrintCrashBanner(const char* errorType, const std::string& details) {
    auto logger = Logger::Get();
    
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
    spdlog::shutdown();
}

} // namespace Acidum