#pragma once

#include <cstdint>

#include "Acidum/Core/Application.hpp"
#include "Acidum/Scene/Camera.hpp"

namespace SandboxConsts {
    inline constexpr float CAMERA_FOV = 45.0f;
    inline constexpr float CAMERA_NEAR_CLIP = 0.1f;
    inline constexpr float CAMERA_FAR_CLIP = 100.0f;

    inline constexpr uint32_t WINDOW_WIDTH = 1280;
    inline constexpr uint32_t WINDOW_HEIGHT = 720;
    inline constexpr const char* WINDOW_TITLE = "Acidum: Sandbox";
}

class SandboxApp : public Acidum::Application {
public:
    SandboxApp(Acidum::APIType apiType);
protected:
    void OnInit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
private:
    float m_totalTime;
    std::unique_ptr<Acidum::IMesh> m_triangleMesh;
    Acidum::Camera m_camera {
        SandboxConsts::CAMERA_FOV,
        static_cast<float>(SandboxConsts::WINDOW_WIDTH) / static_cast<float>(SandboxConsts::WINDOW_HEIGHT),
        SandboxConsts::CAMERA_NEAR_CLIP, SandboxConsts::CAMERA_FAR_CLIP
    };
};