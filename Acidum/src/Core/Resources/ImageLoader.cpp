#include "Acidum/Core/Resources/ImageLoader.hpp"

#include "Acidum/Core/Base/Logger.hpp"
#include "stb_image.h"

namespace Acidum {

ImageData ImageLoader::load(const std::string& path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    ENGINE_VERIFY(pixels, "Failed to load texture: ", path);

    size_t imageSize = static_cast<size_t>(texWidth) * static_cast<size_t>(texHeight) * 4;

    ImageData data {};
    data.pixels.assign(pixels, pixels + imageSize);
    data.width = texWidth;
    data.height = texHeight;
    data.channels = 4;

    stbi_image_free(pixels);

    return data;
}
    
} // namespace Acidum