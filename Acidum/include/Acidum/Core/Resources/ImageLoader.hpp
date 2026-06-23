#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Acidum {

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
};

} // namespace Acidum