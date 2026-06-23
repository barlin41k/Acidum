#pragma once

#include <unordered_map>
#include <filesystem>
#include <vector>
#include <string>

#include "Acidum/Graphics/Interfaces/IGraphicsAPI.hpp"
#include "Acidum/Graphics/Interfaces/ITexture2D.hpp"

namespace Acidum {

class ResourceManager {
public:
    ResourceManager() = delete;

    static void initialize();
    static void shutdown();

    static std::vector<char> loadBinaryFile(const std::string& relativePath);
    static std::shared_ptr<ITexture2D> loadTexture(const std::string& relativePath);
    static std::shared_ptr<ITexture2D> loadTextureFromMemory(const std::vector<uint8_t>& memory);
    static std::shared_ptr<Model> loadModel(const std::string& relativePath);

    static void setGraphicsAPI(IGraphicsAPI* api) { s_graphicsAPI = api; };
private:
    static IGraphicsAPI* s_graphicsAPI;

    static std::filesystem::path s_assetsPath;

    static std::unordered_map<std::string, std::shared_ptr<ITexture2D>> s_textures;
    static std::unordered_map<std::string, std::shared_ptr<Model>> s_models;
};

} // namespace Acidum