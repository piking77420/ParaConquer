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

    RhiIndexBuffer(size_t _sizeInByte, MemoryLocalisation _visibility, PC_CORE::MemoryUsage _memoryUsage) : RhiBuffer(_sizeInByte,_visibility, _memoryUsage)
    {
        
    }
    
    RhiIndexBuffer() : RhiBuffer(0, PC_CORE::MemoryLocalisation::Count, PC_CORE::MemoryUsage::Count)
    {
        
    }

    ~RhiIndexBuffer() override = default;

protected:
    IndexFormat m_IndexFormat = IndexFormat::Uiunt8;
    
    uint32_t m_IndexCount = 0;

};

END_PCCORE