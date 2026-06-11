#pragma once

#include <glm/glm.hpp>

#include "Acidum/Core/Platform/KeyboardCode.hpp"
#include "Acidum/Core/Platform/MouseCode.hpp"

namespace Acidum {

enum class CursorMode {
    Normal = 0,
    Hidden = 1,
    Locked = 2
};

class Input {
public:
    static bool IsKeyPressed(KeyCode key);
    static bool IsMouseButtonPressed(MouseCode button);

    static float GetMouseX();
    static float GetMouseY();
    static glm::vec2 GetMousePosition();

    static void SetCursorMode(CursorMode mode);
};

} // namespace Acidum