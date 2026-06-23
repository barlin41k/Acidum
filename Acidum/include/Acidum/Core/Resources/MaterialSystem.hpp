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

class MaterialSystem {
public:
    static void RegisterTemplate(RenderMode mode, const std::string& vertPath, const std::string& fragPath, bool blend=false, bool depthWrite=true);

    static std::shared_ptr<Material> CreateMaterial(const MeshData& meshData);
private:
    struct TemplateData {
        std::string vertShaderPath;
        std::string fragShaderPath;
        
        bool enableBlending = false;
        bool depthWrite = true;
    };

    static std::unordered_map<RenderMode, TemplateData> m_templates;
};

} // namespace Acidum