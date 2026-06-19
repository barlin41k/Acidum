#include "Acidum/Core/Base/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Acidum {

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init() {
    spdlog::set_pattern("%^[%H:%M:%S.%e] [%l] %v%$");
    s_Logger = spdlog::stdout_color_mt("ENGINE");
    
#ifdef ACIDUM_DEBUG
    s_Logger->set_level(spdlog::level::trace);
#else
    s_Logger->set_level(spdlog::level::info);
#endif
}

} // namespace Acidum