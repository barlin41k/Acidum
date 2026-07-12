#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Acidum {

typedef unsigned char stbi_uc;

struct ImageData {
    std::vector<uint8_t> pixels;
    int width;
    int height;
    int channels;
};
    
class ImageLoader {
public:
    static ImageData load(const std::string& path);
    static ImageData loadFromMemory(const std::vector<uint8_t>& memory);
private:
    static ImageData loadImageData(stbi_uc* pixels, int texWidth, int texHeight);
};

} // namespace Acidum