#include "Graphics/GraphicsFactory.hpp"

#include "Graphics/Vulkan/VulkanGraphicsAPI.hpp" 

std::unique_ptr<IGraphicsAPI> GraphicsFactory::createAPI(APIType type, Window* window) {
    switch (type) {
        case APIType::Vulkan:
            return std::make_unique<VulkanGraphicsAPI>(window);
        default:
            throw std::runtime_error("Unknown Graphics API Type!");
    }
}