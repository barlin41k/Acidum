#include "Acidum/Core/Resources/MaterialSystem.hpp"

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

std::unordered_map<RenderMode, MaterialSystem::TemplateData> MaterialSystem::s_templates;
std::unordered_map<std::string, MaterialSystem::TemplateData> MaterialSystem::s_customTemplates;

void MaterialSystem::RegisterTemplate(RenderMode mode, const std::string& vertPath, const std::string& fragPath, bool enableBlending, bool depthWrite) {
    if (s_templates.contains(mode)) {
        ACIDUM_WARN("Template for RenderMode ({}) already exists! Skipping ({}, {})", RenderModeToString(mode), vertPath, fragPath);
        return;
    }

    TemplateData templateData {};
    templateData.vertShaderPath = vertPath;
    templateData.fragShaderPath = fragPath;
    templateData.enableBlending = enableBlending;
    templateData.depthWrite = depthWrite;

    s_templates[mode] = templateData;
}

std::shared_ptr<Material> MaterialSystem::CreateMaterial(const MeshData& meshData) {
    RenderMode mode = meshData.isTransparent ? RenderMode::Transparent : RenderMode::Opaque;

    auto it = s_templates.find(mode);
    ACIDUM_ASSERT(it != s_templates.end(), "Template for requested RenderMode ({}) not found!", RenderModeToString(mode));

    const auto& templ = it->second;

    return std::make_shared<Material>(Material{
        .vertShaderPath = templ.vertShaderPath,
        .fragShaderPath = templ.fragShaderPath,
        .baseColor = meshData.baseColorFactor,
        .roughness = meshData.roughness,
        .metallic = meshData.metallic,
        .enableBlending = templ.enableBlending,
        .depthWrite = templ.depthWrite
    });
}

void MaterialSystem::RegisterCustomTemplate(const std::string& name, const std::string& vertPath, const std::string& fragPath, bool enableBlending, bool depthWrite) {
    if (s_customTemplates.contains(name)) {
        ACIDUM_WARN("Template {} already exists in custom templates! Skipping ({}, {})", name, vertPath, fragPath);
        return;
    }

    TemplateData templateData {};
    templateData.vertShaderPath = vertPath;
    templateData.fragShaderPath = fragPath;
    templateData.enableBlending = enableBlending;
    templateData.depthWrite = depthWrite;

    s_customTemplates[name] = templateData;
}

std::shared_ptr<Material> MaterialSystem::CreateCustomMaterial(const std::string& templateName) {
    auto it = s_customTemplates.find(templateName);
    ACIDUM_ASSERT(it != s_customTemplates.end(), "Requested custom template {} not found!", templateName);

    const auto& templ = it->second;
    return std::make_shared<Material>(Material{
        .vertShaderPath = templ.vertShaderPath,
        .fragShaderPath = templ.fragShaderPath,
        .enableBlending = templ.enableBlending,
        .depthWrite = templ.depthWrite
    });
}

} // namespace Acidum