#pragma once

#include "Acidum/Core/Application.hpp"

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
};