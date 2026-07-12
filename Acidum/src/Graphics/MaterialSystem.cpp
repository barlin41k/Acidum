#include "Acidum/Graphics/MaterialSystem.hpp"

#include "Acidum/Core/Resources/ResourceManager.hpp"
#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

std::unordered_map<RenderMode, MaterialConfig> MaterialSystem::s_templates;
std::unordered_map<std::string, MaterialConfig> MaterialSystem::s_customTemplates;

void MaterialSystem::RegisterTemplate(RenderMode mode, const MaterialConfig& config) {
    if (s_templates.contains(mode)) {
        ACIDUM_WARN(
            "Template for RenderMode ({}) already exists! Skipping ({}, {})",
            RenderModeHelper::RenderModeToString(mode), config.vertShaderPath, config.fragShaderPath
        );
        return;
    }

    s_templates[mode] = config;
}

std::shared_ptr<Material> MaterialSystem::CreateMaterial(const MeshData& meshData) {
    RenderMode mode = meshData.isTransparent ? RenderMode::Transparent : RenderMode::Opaque;

    auto it = s_templates.find(mode);
    ACIDUM_ASSERT(it != s_templates.end(), "Template for requested RenderMode ({}) not found!", RenderModeHelper::RenderModeToString(mode));

    const auto& config = it->second;
    return std::make_shared<Material>(Material{
        .config = config,
        .albedoTexture = ResourceManager::getMissingTexture(),
        .metallicRoughnessTexture = ResourceManager::getMissingTexture(),
        .normalTexture = ResourceManager::getMissingNormalTexture(),
        .baseColor = meshData.baseColorFactor,
        .roughness = meshData.roughness,
        .metallic = meshData.metallic
    });
}

void MaterialSystem::RegisterCustomTemplate(const std::string& name, const MaterialConfig& config) {
    if (s_customTemplates.contains(name)) {
        ACIDUM_WARN("Template {} already exists in custom templates! Skipping ({}, {})", name, config.vertShaderPath, config.fragShaderPath);
        return;
    }

    s_customTemplates[name] = config;
}

std::shared_ptr<Material> MaterialSystem::CreateCustomMaterial(const std::string& templateName) {
    auto it = s_customTemplates.find(templateName);
    ACIDUM_ASSERT(it != s_customTemplates.end(), "Requested custom template {} not found!", templateName);

    const auto& config = it->second;
    return std::make_shared<Material>(Material{
        .config = config,
        .albedoTexture = ResourceManager::getMissingTexture(),
        .metallicRoughnessTexture = ResourceManager::getMissingTexture(),
        .normalTexture = ResourceManager::getMissingNormalTexture()
    });
}

} // namespace Acidum