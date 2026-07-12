#include "Acidum/Core/Resources/ImageLoader.hpp"

#include "stb_image.h"

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

ImageData ImageLoader::loadImageData(stbi_uc* pixels, int texWidth, int texHeight) {
    size_t imageSize = static_cast<size_t>(texWidth) * static_cast<size_t>(texHeight) * 4;

    ImageData data {};
    data.pixels.assign(pixels, pixels + imageSize);
    data.width = texWidth;
    data.height = texHeight;
    data.channels = 4;

    stbi_image_free(pixels);

    return data;
}

ImageData ImageLoader::load(const std::string& path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    ACIDUM_ASSERT(pixels, "Failed to load texture: ", path);

    ImageData data = loadImageData(pixels, texWidth, texHeight);

    return data;
}

ImageData ImageLoader::loadFromMemory(const std::vector<uint8_t>& memory) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load_from_memory(
        memory.data(), static_cast<int>(memory.size()),
        &texWidth, &texHeight, &texChannels,
        STBI_rgb_alpha
    );

    ACIDUM_ASSERT(pixels, "Failed to load embedded texture from memory!");

    ImageData data = loadImageData(pixels, texWidth, texHeight);

    return data;
}
    
} // namespace Acidum