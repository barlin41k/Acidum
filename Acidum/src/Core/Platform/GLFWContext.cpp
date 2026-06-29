#include "Acidum/Core/Platform/GLFWContext.hpp"

#include <GLFW/glfw3.h>

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

void GLFWContext::Init() {
    ACIDUM_ASSERT(glfwInit(), "Failed to initialize GLFW!");
    glfwSetErrorCallback(errorCallback);
}

void GLFWContext::Shutdown() {
    glfwTerminate();
}

void GLFWContext::errorCallback(int error, const char* description) {
    ACIDUM_ERROR("GLFW encountered an error ({}): {}", error, description);
}
    
} // namespace Acidum