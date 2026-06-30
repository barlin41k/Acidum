#include "Acidum/Core/Base/Logger.hpp"

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Acidum {

std::vector<spdlog::sink_ptr> Logger::s_Sinks;

void Logger::Init() {
    spdlog::init_thread_pool(2048, 1);

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%H:%M:%S.%e] [%n] [%l] %v%$");

    s_Sinks.push_back(consoleSink);
}

std::shared_ptr<spdlog::logger> Logger::Get(const std::string& name) {
    auto logger = spdlog::get(name);
    if (logger) return logger;

    logger = std::make_shared<spdlog::async_logger>(
        name,
        s_Sinks.begin(), s_Sinks.end(),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::overrun_oldest
    );

#ifdef ACIDUM_DEBUG_BUILD
    logger->set_level(spdlog::level::trace);
#else
    logger->set_level(spdlog::level::info);
#endif

    logger->flush_on(spdlog::level::err);
    
    spdlog::register_logger(logger);

    return logger;
}

} // namespace Acidum