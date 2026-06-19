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

#define ENGINE_INFO(...) SPDLOG_LOGGER_INFO(Acidum::Logger::GetLogger(), __VA_ARGS__)
#define ENGINE_WARN(fmt, ...) SPDLOG_LOGGER_WARN(Acidum::Logger::GetLogger(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ENGINE_ERROR(fmt, ...) SPDLOG_LOGGER_ERROR(Acidum::Logger::GetLogger(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ENGINE_FATAL(fmt, ...) SPDLOG_LOGGER_CRITICAL(Acidum::Logger::GetLogger(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)

#define ENGINE_VERIFY(condition, msg, ...) \
    do { \
        if (!(condition)) { \
            ENGINE_FATAL("Engine verification failed: " msg __VA_OPT__(,) __VA_ARGS__); \
            std::abort(); \
        } \
    } while (false)

#ifdef ACIDUM_DEBUG
    #define ENGINE_TRACE(...) SPDLOG_LOGGER_TRACE(Acidum::Logger::GetLogger(), __VA_ARGS__)
    #define ENGINE_DEBUG(...) SPDLOG_LOGGER_DEBUG(Acidum::Logger::GetLogger(), __VA_ARGS__)

    #define ENGINE_ASSERT(condition, msg, ...) \
        do { \
            if (!(condition)) { \
                ENGINE_FATAL("Engine assertion failed: " msg __VA_OPT__(,) __VA_ARGS__); \
                std::abort(); \
            } \
        } while (false)
#else
    #define ENGINE_TRACE(...)
    #define ENGINE_DEBUG(...)

    #define ENGINE_ASSERT(condition, msg, ...)
#endif

} // namespace Acidum