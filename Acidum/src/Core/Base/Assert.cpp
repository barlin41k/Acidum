#include "Acidum/Core/Base/Assert.hpp"

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

void PrintCrashBanner(const char* errorType, const std::string& details) {
    auto logger = Logger::Get();
    
    logger->critical("--------------------------------------------------");
    logger->critical("Acidum Engine crash: [{}]", errorType);
    logger->critical("Details: {}", details);
    logger->critical("--------------------------------------------------");

    logger->flush();
    spdlog::shutdown();
}

} // namespace Acidum