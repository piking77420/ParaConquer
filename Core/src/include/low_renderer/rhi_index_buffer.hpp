#pragma once

#include "core_header.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE
class PC_CORE_API RhiIndexBuffer
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiIndexBuffer)

    IndexFormat GetIndexFormat() const;

    uint32_t GetIndexCount() const;

    RhiIndexBuffer(const uint32_t* _indices, size_t _size);

    RhiIndexBuffer(const uint16_t* _indices, size_t _size);

    RhiIndexBuffer(const uint8_t* _indices, size_t _size);
    
    RhiIndexBuffer() = default;

    ~RhiIndexBuffer() = default;

private:
    IndexFormat m_IndexFormat = IndexFormat::Uint16;
    
    uint32_t m_IndexCount = 0;

};

END_PCCORE