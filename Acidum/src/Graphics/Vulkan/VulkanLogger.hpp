#pragma once

#include "Acidum/Core/Base/Assert.hpp" // IWYU pragma: export
#include "Acidum/Core/Base/Logger.hpp" // IWYU pragma: export

namespace Acidum {

inline std::shared_ptr<spdlog::logger>& GetVulkanLogger() {
    static auto logger = Logger::Get("Vulkan");
    return logger;
}

#ifdef ACIDUM_DEBUG_BUILD
    #define VK_TRACE(str, ...) SPDLOG_LOGGER_TRACE(Acidum::GetVulkanLogger(), str __VA_OPT__(,) __VA_ARGS__)
    #define VK_DEBUG(str, ...) SPDLOG_LOGGER_DEBUG(Acidum::GetVulkanLogger(), str __VA_OPT__(,) __VA_ARGS__)
#else
    #define VK_TRACE(str, ...) do { } while (false)
    #define VK_DEBUG(str, ...) do { } while (false)
#endif

#define VK_INFO(str, ...)  SPDLOG_LOGGER_INFO(Acidum::GetVulkanLogger(), str __VA_OPT__(,) __VA_ARGS__)
#define VK_WARN(fmt, ...)  SPDLOG_LOGGER_WARN(Acidum::GetVulkanLogger(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define VK_ERROR(fmt, ...) SPDLOG_LOGGER_ERROR(Acidum::GetVulkanLogger(), "[{}:{}] " fmt, Acidum::GetShortFileName(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)

} // namespace Acidum