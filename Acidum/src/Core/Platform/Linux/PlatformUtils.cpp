#include "Acidum/Core/Platform/PlatformUtils.hpp"

#include <unistd.h>
#include <limits.h>

namespace Acidum::Platform {

std::filesystem::path GetExecutableDir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1)
        return std::filesystem::path(std::string_view(result, static_cast<size_t>(count))).parent_path();
    return {};
}

} // namespace Acidum::Platform