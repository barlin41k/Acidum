#pragma once

#include "Acidum/Core/Application.hpp"

class SandboxApp : public Application {
public:
    SandboxApp(APIType apiType);
protected:
    void OnInit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
private:
    float m_totalTime;
    std::unique_ptr<IMesh> m_triangleMesh;
};