#pragma once

#include "core_header.hpp"
#include "rhi_buffer.h"
#include "rhi_typedef.h"

BEGIN_PCCORE
class PC_CORE_API RhiIndexBuffer : public RhiBuffer
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiIndexBuffer)

    IndexFormat GetIndexFormat() const;

    uint32_t GetIndexCount() const;
    
    RhiIndexBuffer() = default;

    ~RhiIndexBuffer() override = default;

protected:
    IndexFormat m_IndexFormat = IndexFormat::Uint16;
    
    uint32_t m_IndexCount = 0;

};

END_PCCORE