#pragma once

#include <cstdint>

#include "Acidum/Core/Application.hpp"
#include "Acidum/Scene/Camera.hpp"
#include "Acidum/Scene/Entity.hpp"

namespace SandboxConsts {
    inline constexpr float CAMERA_FOV = 45.0f;
    inline constexpr float CAMERA_NEAR_CLIP = 0.1f;
    inline constexpr float CAMERA_FAR_CLIP = 100.0f;
    inline constexpr float CAMERA_SPEED = 2.0f;

    inline constexpr uint32_t WINDOW_WIDTH = 1280;
    inline constexpr uint32_t WINDOW_HEIGHT = 720;
    inline constexpr const char* WINDOW_TITLE = "Sandbox";
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

    bool m_firstMouse = true;
    glm::vec2 m_lastMousePos{0.0f, 0.0f};
    glm::vec3 m_cameraPos{0.0f, 0.0f, 3.0f};
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    Acidum::Camera m_camera {
        SandboxConsts::CAMERA_FOV,
        static_cast<float>(SandboxConsts::WINDOW_WIDTH) / static_cast<float>(SandboxConsts::WINDOW_HEIGHT),
        SandboxConsts::CAMERA_NEAR_CLIP, SandboxConsts::CAMERA_FAR_CLIP
    };

    float m_fpsTimer = 0.0f;
    uint32_t m_frameCount = 0;

    std::vector<Acidum::Entity> m_entities;
    
    void updateWindowTitle(float deltaTime);
    void updateCamera(float deltaTime);
    void updateShaderMatrices();
};