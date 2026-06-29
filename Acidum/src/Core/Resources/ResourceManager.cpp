#include "Acidum/Core/Resources/ResourceManager.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Core/Platform/PlatformUtils.hpp"
#include "Acidum/Core/Resources/MaterialSystem.hpp"
#include "Acidum/Core/Resources/ImageLoader.hpp"
#include "Acidum/Core/Resources/ModelLoader.hpp"

namespace Acidum {

IGraphicsAPI* ResourceManager::s_graphicsAPI;
std::filesystem::path ResourceManager::s_assetsPath;
std::shared_ptr<ITexture2D> ResourceManager::s_missingTexture = nullptr;
std::shared_ptr<ITexture2D> ResourceManager::s_missingNormalTexture = nullptr;
std::unordered_map<std::string, std::shared_ptr<ITexture2D>> ResourceManager::s_textures;
std::unordered_map<std::string, std::shared_ptr<Model>> ResourceManager::s_models;

void ResourceManager::initialize() {
    std::filesystem::path executeDir = Platform::GetExecutableDir();

    ACIDUM_ASSERT(!executeDir.empty(), "Failed to determine executable directory!");

    s_assetsPath = executeDir / "assets";

    ACIDUM_INFO("ResourceManager initialized with base path: {}", s_assetsPath.string());
}

void ResourceManager::shutdown() {
    s_textures.clear();
    s_models.clear();

    s_missingTexture.reset();
    s_missingNormalTexture.reset();
    
    s_graphicsAPI = nullptr;

    ACIDUM_INFO("ResourceManager shutdown successfully!");
}

std::shared_ptr<ITexture2D> ResourceManager::getMissingTexture() {
    if (!s_missingTexture) {
        uint8_t pixels[4] = { 255, 255, 255, 255 }; 
        s_missingTexture = s_graphicsAPI->createTexture2D(pixels, 1, 1);
    }

    return s_missingTexture;
}

std::shared_ptr<ITexture2D> ResourceManager::getMissingNormalTexture() {
    if (!s_missingNormalTexture) {
        uint8_t pixels[4] = { 128, 128, 255, 255 };
        s_missingNormalTexture = s_graphicsAPI->createTexture2D(pixels, 1, 1);
    }

    return s_missingNormalTexture;
}

std::vector<char> ResourceManager::loadBinaryFile(const std::string& relativePath) {
    std::filesystem::path fullPath = s_assetsPath / relativePath;
    
    std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

    ACIDUM_ASSERT(file.is_open(), "Failed to open file: {}", fullPath.string());
    
    std::streampos pos = file.tellg();
    ACIDUM_ASSERT(pos != -1, "Failed to get file size for: {}", fullPath.string());
    size_t fileSize = static_cast<size_t>(pos);
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    return buffer;
}

std::shared_ptr<ITexture2D> ResourceManager::loadTexture(const std::string& relativePath, bool isSRGB) {
    if (s_textures.contains(relativePath))
        return s_textures[relativePath];

    std::filesystem::path fullPath = s_assetsPath / relativePath;

    ImageData data = ImageLoader::load(fullPath);

    auto texture = s_graphicsAPI->createTexture2D(
        data.pixels.data(),
        static_cast<uint32_t>(data.width),
        static_cast<uint32_t>(data.height),
        isSRGB
    );

    s_textures[relativePath] = std::move(texture);
    return s_textures[relativePath];
}

std::shared_ptr<ITexture2D> ResourceManager::loadTextureFromMemory(const std::vector<uint8_t>& memory, bool isSRGB) {
    ImageData data = ImageLoader::loadFromMemory(memory);
    return s_graphicsAPI->createTexture2D(
        data.pixels.data(),
        static_cast<uint32_t>(data.width),
        static_cast<uint32_t>(data.height),
        isSRGB
    );
}

std::shared_ptr<Model> ResourceManager::loadModel(const std::string& relativePath) {
    if (s_models.contains(relativePath))
        return s_models[relativePath];

    std::filesystem::path fullPath = s_assetsPath / relativePath;
    std::vector<MeshData> subMeshesData = ModelLoader::load(fullPath.string());

    auto model = std::make_shared<Model>();
    for (const auto& data : subMeshesData) {
        auto mesh = s_graphicsAPI->createMesh(data.vertices, data.indices);
        auto material = MaterialSystem::CreateMaterial(data);

        

        if (!data.embeddedImage.empty())
            material->albedoTexture = loadTextureFromMemory(data.embeddedImage);
        else if (!data.textureName.empty()) {
            std::filesystem::path relTexPath = std::filesystem::path(relativePath).parent_path() / data.textureName;
            material->albedoTexture = loadTexture(relTexPath.string());
        } else 
            material->albedoTexture = getMissingTexture();
        
        if (!data.embeddedMetallicRoughnessImage.empty())
            material->metallicRoughnessTexture = loadTextureFromMemory(data.embeddedMetallicRoughnessImage, false);
        else if (!data.metallicRoughnessTextureName.empty()) {
            std::filesystem::path relTexPath = std::filesystem::path(relativePath).parent_path() / data.metallicRoughnessTextureName;
            material->metallicRoughnessTexture = loadTexture(relTexPath.string(), false);
        } else
            material->metallicRoughnessTexture = getMissingTexture();
        
        if (!data.embeddedNormalImage.empty())
            material->normalTexture = loadTextureFromMemory(data.embeddedNormalImage, false);
        else if (!data.normalTextureName.empty()) {
            std::filesystem::path relTexPath = std::filesystem::path(relativePath).parent_path() / data.normalTextureName;
            material->normalTexture = loadTexture(relTexPath.string(), false);
        } else
            material->normalTexture = getMissingNormalTexture();

        mesh->setMaterial(material);

        MeshNode node;
        node.name = data.name;
        node.mesh = std::move(mesh);
        node.localTransform = data.transform;

        model->nodes.push_back(std::move(node));
    }

    s_models[relativePath] = model;
    return model;
}

} // namespace Acidum