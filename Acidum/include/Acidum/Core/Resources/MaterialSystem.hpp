#pragma once

#include <memory>
#include <string>

#include "Acidum/Graphics/Material.hpp"
#include "Acidum/Core/Resources/ModelLoader.hpp"

namespace Acidum {

enum class RenderMode {
    Opaque,
    Transparent
};

constexpr std::string_view RenderModeToString(RenderMode mode) {
    switch (mode) {
        case RenderMode::Opaque: return "Opaque";
        case RenderMode::Transparent: return "Transparent";
        default: return "Unknown";
    }
}

class MaterialSystem {
public:
    static void RegisterTemplate(RenderMode mode, const std::string& vertPath, const std::string& fragPath, bool blend=false, bool depthWrite=true);
    static void RegisterCustomTemplate(const std::string& name, const std::string& vertPath, const std::string& fragPath, bool blend=false, bool depthWrite=true);

    static std::shared_ptr<Material> CreateMaterial(const MeshData& meshData);
    static std::shared_ptr<Material> CreateCustomMaterial(const std::string& templateName);
private:
    struct TemplateData {
        std::string vertShaderPath;
        std::string fragShaderPath;
        
        bool enableBlending = false;
        bool depthWrite = true;
    };

    static std::unordered_map<RenderMode, TemplateData> s_templates;
    static std::unordered_map<std::string, TemplateData> s_customTemplates;
};

} // namespace Acidum