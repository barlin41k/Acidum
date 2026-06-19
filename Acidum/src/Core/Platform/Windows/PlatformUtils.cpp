#include "Acidum/Core/Platform/PlatformUtils.hpp"

// TODO: check the functionality of this code

#include <windows.h>

namespace Acidum::Platform {

std::filesystem::path GetExecutableDir() {
    char result[MAX_PATH];
    if (GetModuleFileNameA(NULL, result, MAX_PATH) != 0)
        return = std::filesystem::path(result).parent_path();
    return {};
}

} // namespace Acidum::Platform