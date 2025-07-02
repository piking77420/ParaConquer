#pragma once

#include "core_header.hpp"
#include "gpu_buffer.hpp"
#include "low_renderer/rhi_buffer.h"
#include "low_renderer/rhi_vertex_buffer.hpp"
#include "low_renderer/vertex.hpp"

BEGIN_PCCORE

PC_CORE_API class VertexBuffer final : public GpuBuffer
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
    
    DEFAULT_COPY_MOVE_OPERATIONS(VertexBuffer)

    VertexBuffer(Vertex* _vertices, size_t _count, PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage);

    VertexBuffer() = default;

    ~VertexBuffer() override = default;
private:
    size_t m_Count = 0;

    std::shared_ptr<RhiVertexBuffer> m_RhiBuffer;
};

END_PCCORE