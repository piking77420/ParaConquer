#pragma once

#include "CoreHeader.hpp"
#include "GpuBuffer.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiVertexBuffer.hpp"
#include "LowRenderer/Vertex.hpp"

BEGIN_PCCORE

class VertexBuffer final : public GpuBuffer
{
public:

    std::shared_ptr<RhiResource> GetRhiHandle() const override
    {
        return m_RhiBuffer;
    }
    
    size_t GetCount() const
    {
        return m_Count;
    }

    std::shared_ptr<RhiVertexBuffer> GetRhiBuffer() const
    {
        return m_RhiBuffer;     
    }

    PC_CORE_API void Update(void* _data, size_t _size) const;
    
    DEFAULT_COPY_MOVE_OPERATIONS(VertexBuffer)

    PC_CORE_API VertexBuffer(Vertex* _vertices, size_t _count, PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage);

    PC_CORE_API VertexBuffer(size_t _sizeInBytes, PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage);
    
    PC_CORE_API VertexBuffer() = default;

    PC_CORE_API ~VertexBuffer() override = default;
private:
    size_t m_Count = 0;

    std::shared_ptr<RhiVertexBuffer> m_RhiBuffer;
};

END_PCCORE