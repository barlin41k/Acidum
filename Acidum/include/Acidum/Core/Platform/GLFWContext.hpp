#pragma once

namespace Acidum {

class GLFWContext {
public:
    static void Init();
    static void Shutdown();
private:
    static void errorCallback(int error, const char* description);
};

} // namespace Acidum