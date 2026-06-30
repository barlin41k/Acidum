#pragma once

#include <spdlog/fmt/fmt.h>

#include <cstdlib> // IWYU pragma: keep
#include <string>

namespace Acidum {

void PrintCrashBanner(const char* errorType, const std::string& details);

constexpr const char* GetShortFileName(const char* path) {
    const char* file = path;
    while (*path) {
        if (*path == '/' || *path == '\\')
            file = path + 1;
        path++;
    }

    return file;
}

#define ACIDUM_ASSERT(condition, msg, ...) \
    do { \
        if (!(condition)) { \
            std::string details = fmt::format("[{}:{}] " msg, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__); \
            Acidum::PrintCrashBanner("Assertion Failed", details); \
            std::abort(); \
        } \
    } while (false) \

} // namespace Acidum