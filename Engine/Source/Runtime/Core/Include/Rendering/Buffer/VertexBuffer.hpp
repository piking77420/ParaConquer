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

        PC_CORE_API explicit VertexBuffer(PC_CORE::Rhi& rhi, const std::string& _name, size_t _vertexCount, size_t _verticiesSize);
    
        PC_CORE_API explicit VertexBuffer(PC_CORE::Rhi& rhi, const std::string& _name, size_t _sizeInBytes);
    
        PC_CORE_API VertexBuffer() = default;

        PC_CORE_API ~VertexBuffer() override = default;

        size_t GetVerticiesCount() const
        {
            return m_Count;
        }

        size_t GetVerticiesSize() const
        {
            return m_VerticiesSize;
        }


    private:
        size_t m_Count = 0;
    
        size_t m_VerticiesSize = 0;
    };

END_PCCORE
