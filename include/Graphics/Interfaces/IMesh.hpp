#pragma once

#include <cstdint>

class IMesh {
public:
    virtual ~IMesh() = default;

    virtual uint32_t getVertexCount() const = 0;
    virtual uint32_t getIndexCount() const = 0;
};