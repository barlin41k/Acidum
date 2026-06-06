#include <vulkan/vulkan.h>

#include <sys/types.h>
#include <iostream>
#include <cstdlib>

#include "Core/Consts.hpp"
#include "Core/Application.hpp"

int main() {
    Application app (Consts::DEFAULT_API);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}