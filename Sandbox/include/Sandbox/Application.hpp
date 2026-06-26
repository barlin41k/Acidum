#pragma once

#include <cstdint>

#include "Acidum/Core/Application.hpp"
#include "Acidum/Scene/Camera.hpp"
#include "Acidum/Scene/Entity.hpp"

#include "Sandbox/Scene/Controllers/CameraController.hpp"

namespace Sandbox {

class Application : public Acidum::Application {
public:
    Application(Acidum::APIType apiType);
protected:
    void OnInit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
private:
    float m_totalTime = 0.0f;

    float m_fpsTimer = 0.0f;
    uint32_t m_frameCount = 0;

    Acidum::Camera m_camera;
    CameraController m_cameraController;

    std::vector<Acidum::Entity> m_entities;

    int m_akMagIndex;
    
    void updateWindowTitle(float deltaTime);
    void updateViewProjMatrices();
};

} // namespace Sandbox