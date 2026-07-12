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

struct RenderModeHelper {
    static constexpr std::string_view RenderModeToString(RenderMode mode) {
        switch (mode) {
            case RenderMode::Opaque: return "Opaque";
            case RenderMode::Transparent: return "Transparent";
            default: return "Unknown";
        }
    }
};

class MaterialSystem {
public:
    static void RegisterTemplate(RenderMode mode, const MaterialConfig& config);
    static void RegisterCustomTemplate(const std::string& name, const MaterialConfig& config);

    static std::shared_ptr<Material> CreateMaterial(const MeshData& meshData);
    static std::shared_ptr<Material> CreateCustomMaterial(const std::string& templateName);
private:
    static std::unordered_map<RenderMode, MaterialConfig> s_templates;
    static std::unordered_map<std::string, MaterialConfig> s_customTemplates;
};

} // namespace Acidum