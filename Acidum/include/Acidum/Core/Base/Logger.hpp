#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "Acidum/Core/Base/Assert.hpp" // IWYU pragma: export

namespace Acidum {

class Logger {
public:
    static void Init();
    static std::shared_ptr<spdlog::logger> Get(const std::string& name="Acidum");
private:
    static std::vector<spdlog::sink_ptr> s_Sinks;
};

#ifdef ACIDUM_DEBUG_BUILD
    #define ACIDUM_TRACE(str, ...) SPDLOG_LOGGER_TRACE(Acidum::Logger::Get(), str __VA_OPT__(,) __VA_ARGS__)
    #define ACIDUM_DEBUG(str, ...) SPDLOG_LOGGER_DEBUG(Acidum::Logger::Get(), str __VA_OPT__(,) __VA_ARGS__)
#else
    #define ACIDUM_TRACE(str, ...) do { } while (false)
    #define ACIDUM_DEBUG(str, ...) do { } while (false)
#endif

#define ACIDUM_INFO(str, ...) SPDLOG_LOGGER_INFO(Acidum::Logger::Get(), str __VA_OPT__(,) __VA_ARGS__)
#define ACIDUM_WARN(fmt, ...) SPDLOG_LOGGER_WARN(Acidum::Logger::Get(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ACIDUM_ERROR(fmt, ...) SPDLOG_LOGGER_ERROR(Acidum::Logger::Get(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ACIDUM_FATAL(fmt, ...) SPDLOG_LOGGER_CRITICAL(Acidum::Logger::Get(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)

} // namespace Acidum