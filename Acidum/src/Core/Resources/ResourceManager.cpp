#include "Acidum/Core/Resources/ResourceManager.hpp"
#include "Acidum/Core/Base/Logger.hpp"

#include <filesystem>
#include <fstream>

#if defined (__linux__)
    #include <sys/types.h>
    #include <unistd.h>
    #include <limits.h>
#elif defined(_WIN32)
    #include <windows.h>
#endif

namespace Acidum {

std::filesystem::path ResourceManager::s_assetsPath;

void ResourceManager::initialize() {
    std::filesystem::path executeDir;

#if defined (__linux__)
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1)
        executeDir = std::filesystem::path(std::string_view(result, static_cast<size_t>(count))).parent_path();
#elif defined(_WIN32)
    char result[MAX_PATH];
    if (GetModuleFileNameA(NULL, result, MAX_PATH) != 0)
        executeDir = std::filesystem::path(result).parent_path();
#endif

    if (executeDir.empty()) {
#if defined(__linux__)
        ENGINE_ERROR("Failed to determine executable directory via /proc/self/exe. System error: {}", std::generic_category().message(errno));
#elif defined(_WIN32)
        ENGINE_ERROR("Failed to determine executable directory via GetModuleFileNameA. Windows error code: {}", GetLastError());
#endif
        executeDir = std::filesystem::current_path();
    }

    s_assetsPath = executeDir / "assets";

    ENGINE_INFO("ResourceManager initialized with base path: {}", s_assetsPath.string());
}

std::vector<char> ResourceManager::loadBinaryFile(const std::string& relativePath) {
    std::filesystem::path fullPath = s_assetsPath / relativePath;
    
    std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

    ENGINE_VERIFY(file.is_open(), "Failed to open file: {}", fullPath.string());
    
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    return buffer;
}

} // namespace Acidum