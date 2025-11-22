#pragma once

#include "CoreHeader.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/Vertex.hpp"

BEGIN_PCCORE
    class RhiVertexBuffer : public RhiBuffer
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(RhiVertexBuffer)

        PC_CORE_API RhiVertexBuffer(size_t _sizeInByte, MemoryLocalisation _visibility, MemoryUsage _memoryUsage);

        PC_CORE_API RhiVertexBuffer() : RhiBuffer(0, MemoryLocalisation::Count, MemoryUsage::Count)
        {
        }

        ~RhiVertexBuffer() override = default;
    };

END_PCCORE
