#pragma once

class IGraphicsAPI {
public:
    virtual ~IGraphicsAPI() = default;

    virtual void initialize() = 0;
    virtual void renderFrame() = 0;

    virtual void waitIdle() const = 0;
};