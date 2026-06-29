#include "Acidum/Graphics/GraphicsFactory.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp"

namespace Acidum {

std::unique_ptr<IGraphicsAPI> GraphicsFactory::createAPI(APIType type, Window* window) {
    switch (type) {
        case APIType::Vulkan:
            return std::make_unique<VulkanGraphicsAPI>(window);
        default:
            ACIDUM_ASSERT(false, "Unknown Graphics API Type!");
            return nullptr;
    }
}

} // namespace Acidum