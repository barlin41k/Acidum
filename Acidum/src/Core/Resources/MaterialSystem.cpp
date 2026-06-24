#include "Acidum/Core/Resources/MaterialSystem.hpp"

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

std::unordered_map<RenderMode, MaterialSystem::TemplateData> MaterialSystem::m_templates;

void MaterialSystem::RegisterTemplate(RenderMode mode, const std::string& vertPath, const std::string& fragPath, bool enableBlending, bool depthWrite) {
    if (m_templates.contains(mode)) return;

    TemplateData templateData;
    templateData.vertShaderPath = vertPath;
    templateData.fragShaderPath = fragPath;
    templateData.enableBlending = enableBlending;
    templateData.depthWrite = depthWrite;

    m_templates[mode] = templateData;
}

std::shared_ptr<Material> MaterialSystem::CreateMaterial(const MeshData& meshData) {
    RenderMode mode = meshData.isTransparent ? RenderMode::Transparent : RenderMode::Opaque;

    auto it = m_templates.find(mode);
    ENGINE_VERIFY(it != m_templates.end(), "MaterialSystem: Template for requested RenderMode not found!");

    const auto& templ = it->second;
    return std::make_shared<Material>(
        templ.vertShaderPath, templ.fragShaderPath,
        meshData.baseColorFactor,
        meshData.roughness, meshData.metallic,
        templ.enableBlending,
        templ.depthWrite
    );
}

} // namespace Acidum
