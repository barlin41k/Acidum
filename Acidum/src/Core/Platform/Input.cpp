#include "Acidum/Core/Platform/Input.hpp"

#include <GLFW/glfw3.h>

#include "Acidum/Core/Application.hpp"
#include "Acidum/Core/Platform/MouseCode.hpp"

namespace Acidum {

void Input::SetCursorMode(CursorMode mode) {
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->getWindow());
    
    switch (mode) {
        case CursorMode::Normal:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case CursorMode::Hidden:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case CursorMode::Locked:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
    }
}

bool Input::IsKeyPressed(KeyCode key) {
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->getWindow());
    auto state = glfwGetKey(window, static_cast<int>(key));

    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(MouseCode button) {
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->getWindow());
    auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));

    return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition() {
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->getWindow());
    double x, y;

    glfwGetCursorPos(window, &x, &y);

    return {static_cast<float>(x), static_cast<float>(y)};
}

float Input::GetMouseX() {
    return GetMousePosition().x;
}

float Input::GetMouseY() {
    return GetMousePosition().y;
}

} // namespace Acidum