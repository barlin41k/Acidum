#pragma once

namespace Acidum::SystemInfo {

constexpr const char* GetOS() {
#if defined (_WIN32)
    return "Windows";
#elif defined (__APPLE__)
    return "macOS";
#elif defined (__linux__)
    return "Linux";
#else
    return "Unknown OS";
#endif
}

constexpr const char* GetCompiler() {
#if defined(__clang__)
    return "Clang " __clang_version__;
#elif defined(__GNUC__)
    return "GCC";
#elif defined(_MSC_VER)
    return "MSVC";
#else
    return "Unknown Compiler";
#endif
}

constexpr const char* GetArchitecture() {
#if defined(__x86_64__) || defined(_M_X64)
    return "x86_64 (64-bit)";
#elif defined(__aarch64__) || defined(_M_ARM64)
    return "ARM64";
#else
    return "Unknown Architecture";
#endif
}

constexpr const char* GetBuildType() {
#if defined(ACIDUM_DEBUG)
    return "Debug";
#else
    return "Release";
#endif
}

} // namespace Acidum::SystemInfo