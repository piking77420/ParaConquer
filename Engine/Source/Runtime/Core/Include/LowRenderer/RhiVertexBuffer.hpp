#pragma once

#include "CoreHeader.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/Vertex.hpp"

BEGIN_PCCORE

class RhiVertexBuffer : public RhiBuffer
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiVertexBuffer)
    
    PC_CORE_API RhiVertexBuffer(size_t _sizeInByte, PC_CORE::MemoryLocalisation _visibility, MemoryUsage _memoryUsage);
    
    PC_CORE_API RhiVertexBuffer() : RhiBuffer(0, PC_CORE::MemoryLocalisation::Count, PC_CORE::MemoryUsage::Count)
    {
        
    }
    
    ~RhiVertexBuffer() override = default;
private:
};

END_PCCORE