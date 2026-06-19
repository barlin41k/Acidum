#include "Acidum/Core/Resources/ResourceManager.hpp"

#include <filesystem>
#include <fstream>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/PlatformUtils.hpp"

namespace Acidum {

std::filesystem::path ResourceManager::s_assetsPath;

void ResourceManager::initialize() {
    std::filesystem::path executeDir = Platform::GetExecutableDir();

    if (executeDir.empty())
        ENGINE_FATAL("Failed to determine executable directory!");

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