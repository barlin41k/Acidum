#include "Core/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init() {
    spdlog::set_pattern("%^[%H:%M:%S.%e] [%l] %v%$");
    s_Logger = spdlog::stdout_color_mt("ENGINE");
#ifdef NDEBUG
    s_Logger->set_level(spdlog::level::info);
#else
    s_Logger->set_level(spdlog::level::trace);
#endif
}