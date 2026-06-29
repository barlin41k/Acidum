#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <cstdlib> // IWYU pragma: keep
#include <memory>

namespace Acidum {

class Logger {
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

    static void PrintCrashBanner(const char* errorType, const std::string& details);
private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

constexpr const char* GetShortFileName(const char* path) {
    const char* file = path;
    while (*path) {
        if (*path == '/' || *path == '\\')
            file = path + 1;
        path++;
    }

    return file;
}

#ifdef ACIDUM_DEBUG_BUILD
    #define ACIDUM_TRACE(str, ...) SPDLOG_LOGGER_TRACE(Acidum::Logger::GetLogger(), "[ACIDUM] " str __VA_OPT__(,) __VA_ARGS__)
    #define ACIDUM_DEBUG(str, ...) SPDLOG_LOGGER_DEBUG(Acidum::Logger::GetLogger(), "[ACIDUM] " str __VA_OPT__(,) __VA_ARGS__)
#else
    #define ACIDUM_TRACE(str, ...) do { } while (false)
    #define ACIDUM_DEBUG(str, ...) do { } while (false)
#endif

#define ACIDUM_INFO(str, ...) SPDLOG_LOGGER_INFO(Acidum::Logger::GetLogger(), "[ACIDUM] " str __VA_OPT__(,) __VA_ARGS__)
#define ACIDUM_WARN(fmt, ...) SPDLOG_LOGGER_WARN(Acidum::Logger::GetLogger(), "[ACIDUM] [{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ACIDUM_ERROR(fmt, ...) SPDLOG_LOGGER_ERROR(Acidum::Logger::GetLogger(), "[ACIDUM] [{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ACIDUM_FATAL(fmt, ...) SPDLOG_LOGGER_CRITICAL(Acidum::Logger::GetLogger(), "[ACIDUM] [{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)

#define ACIDUM_ASSERT(condition, msg, ...) \
    do { \
        if (!(condition)) { \
            std::string details = fmt::format("[ACIDUM] [{}:{}] " msg, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__); \
            Acidum::Logger::PrintCrashBanner("Assertion Failed", details); \
            std::abort(); \
        } \
    } while (false) \

} // namespace Acidum