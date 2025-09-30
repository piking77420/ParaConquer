#pragma once

#include "RhiBuffer.h"
#include "LowRenderer/RhiUniformBuffer.hpp"

BEGIN_PCCORE

class RhiUniformBuffer : public RhiBuffer
{
public:
    
    const void* GetNativeHandle() const override = 0;
    
    void MapData(void** _ptr) override
    {
        
    }
    
    void UnmapData() override
    {
        
    }

    DEFAULT_COPY_MOVE_OPERATIONS(RhiUniformBuffer)

    PC_CORE_API RhiUniformBuffer(size_t _sizeInByte, PC_CORE::MemoryLocalisation _localisation, PC_CORE::MemoryUsage memoryUsage) : RhiBuffer(_sizeInByte,_localisation, memoryUsage)
    {
        
    }
    PC_CORE_API RhiUniformBuffer() : RhiBuffer()
    {
        
    }
    
    
    PC_CORE_API virtual ~RhiUniformBuffer() = default;
private:
    size_t m_Size = 0;
};

END_PCCORE