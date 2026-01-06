#pragma once

#include "CoreHeader.hpp"
#include "GpuBuffer.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/Vertex.hpp"

BEGIN_PCCORE
    class VertexBuffer final : public GpuBuffer
    {
    public:
    
        DEFAULT_COPY_MOVE_OPERATIONS(VertexBuffer)

        PC_CORE_API explicit VertexBuffer(PC_CORE::Rhi& rhi);
    
        PC_CORE_API VertexBuffer() = default;

        PC_CORE_API ~VertexBuffer() override = default;

        VertexBuffer& SetVerticiesCount(size_t _VerticiesCount)
        {
            m_VerticiesCount = _VerticiesCount;
            return *this;
        }

        VertexBuffer& SetVerticiesSize(size_t _VerticiesSize)
        {
            m_VerticiesSize = _VerticiesSize;
            return *this;
        }


        size_t GetVerticiesCount() const
        {
            return m_VerticiesCount;
        }

        size_t GetVerticiesSize() const
        {
            return m_VerticiesSize;
        }


    private:
        size_t m_VerticiesCount = 0;
    
        size_t m_VerticiesSize = 0;
    };

END_PCCORE
