#include "Acidum/Graphics/GraphicsFactory.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "Acidum/Graphics/Vulkan/VulkanGraphicsAPI.hpp"

namespace Acidum {
std::unique_ptr<IGraphicsAPI> GraphicsFactory::createAPI(APIType type, Window* window) {
    switch (type) {
        case APIType::Vulkan:
            return std::make_unique<VulkanGraphicsAPI>(window);
        default:
            ENGINE_VERIFY(false, "Unknown Graphics API Type!");
            return nullptr;
    }
}
} // namespace Acidum