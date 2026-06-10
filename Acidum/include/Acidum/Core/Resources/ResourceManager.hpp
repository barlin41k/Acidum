#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace Acidum {

class ResourceManager {
public:
    ResourceManager() = delete;

    static void initialize();

    static std::vector<char> loadBinaryFile(const std::string& relativePath);
private:
    static std::filesystem::path s_assetsPath;
};

} // namespace Acidum