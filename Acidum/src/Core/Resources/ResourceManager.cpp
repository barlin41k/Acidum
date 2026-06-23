#include "Acidum/Core/Resources/ResourceManager.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/PlatformUtils.hpp"
#include "Acidum/Core/Resources/ImageLoader.hpp"
#include "Acidum/Core/Resources/ModelLoader.hpp"

namespace Acidum {

IGraphicsAPI* ResourceManager::s_graphicsAPI;
std::filesystem::path ResourceManager::s_assetsPath;
std::unordered_map<std::string, std::shared_ptr<ITexture2D>> ResourceManager::s_textures;
std::unordered_map<std::string, std::shared_ptr<Model>> ResourceManager::s_models;

void ResourceManager::initialize() {
    std::filesystem::path executeDir = Platform::GetExecutableDir();

    if (executeDir.empty())
        ENGINE_FATAL("Failed to determine executable directory!");

    s_assetsPath = executeDir / "assets";

    ENGINE_INFO("ResourceManager initialized with base path: {}", s_assetsPath.string());
}

void ResourceManager::shutdown() {
    s_textures.clear();
    s_models.clear();
    
    s_graphicsAPI = nullptr;

    ENGINE_INFO("ResourceManager shutdown successfully!");
}

std::vector<char> ResourceManager::loadBinaryFile(const std::string& relativePath) {
    std::filesystem::path fullPath = s_assetsPath / relativePath;
    
    std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

    ENGINE_VERIFY(file.is_open(), "Failed to open file: {}", fullPath.string());
    
    std::streampos pos = file.tellg();
    ENGINE_VERIFY(pos != -1, "Failed to get file size for: {}", fullPath.string());
    size_t fileSize = static_cast<size_t>(pos);
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    return buffer;
}

std::shared_ptr<ITexture2D> ResourceManager::loadTexture(const std::string& relativePath) {
    if (s_textures.contains(relativePath))
        return s_textures[relativePath];

    std::filesystem::path fullPath = s_assetsPath / relativePath;

    ImageData data = ImageLoader::load(fullPath);

    auto texture = s_graphicsAPI->createTexture2D(
        data.pixels.data(),
        static_cast<uint32_t>(data.width),
        static_cast<uint32_t>(data.height)
    );

    s_textures[relativePath] = std::move(texture);
    return s_textures[relativePath];
}

std::shared_ptr<Model> ResourceManager::loadModel(const std::string& relativePath, const std::string& vertShaderPath, const std::string& fragShaderPath) {
    if (s_models.contains(relativePath))
        return s_models[relativePath];

    std::filesystem::path fullPath = s_assetsPath / relativePath;
    std::vector<MeshData> subMeshesData = ModelLoader::load(fullPath.string());

    auto model = std::make_shared<Model>();
    for (const auto& data : subMeshesData) {
        auto mesh = s_graphicsAPI->createMesh(data.vertices, data.indices);
        auto material = std::make_shared<Material>(vertShaderPath, fragShaderPath);

        if (!data.textureName.empty()) {
            std::filesystem::path relTexPath = std::filesystem::path(relativePath).parent_path() / data.textureName;
            material->albedoTexture = loadTexture(relTexPath.string());
        }

        mesh->setMaterial(material);
        model->subMeshes.push_back(std::move(mesh));
    }

    s_models[relativePath] = model;
    return model;
}

} // namespace Acidum