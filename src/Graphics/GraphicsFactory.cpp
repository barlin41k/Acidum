#include "Graphics/GraphicsFactory.hpp"

#include "Core/Logger.hpp"
#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp"

std::unique_ptr<IGraphicsAPI> GraphicsFactory::createAPI(APIType type, Window* window) {
    switch (type) {
        case APIType::Vulkan:
            return std::make_unique<VulkanGraphicsAPI>(window);
        default:
            ENGINE_VERIFY(false, "Unknown Graphics API Type!");
            return nullptr;
    }
}