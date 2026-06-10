#pragma once

#include "Acidum/Core/Application.hpp"
#include "Acidum/Scene/Camera.hpp"

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
    Acidum::Camera m_camera{45.0f, 1280.0f / 720.0f, 0.1f, 100.0f};
};