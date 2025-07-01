#pragma once

#include "rhi_buffer.h"
#include "low_renderer/rhi_uniform_buffer.hpp"

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

    PC_CORE_API RhiUniformBuffer(size_t _sizeInByte, PC_CORE::BufferMemoryUsage memoryUsage) : RhiBuffer(_sizeInByte, memoryUsage)
    {
        
    }
    PC_CORE_API RhiUniformBuffer() : RhiBuffer(0, PC_CORE::BufferMemoryUsage::Count)
    {
        
    }
    
    
    PC_CORE_API virtual ~RhiUniformBuffer() = default;
private:
    size_t m_Size = 0;
};

END_PCCORE