#include "Rendering/Buffer/VertexBuffer.hpp"

#include "LowRenderer/Rhi.hpp"



PC_CORE::VertexBuffer::VertexBuffer(PC_CORE::Rhi& rhi, const std::string& _name, size_t _vertexCount, size_t _verticiesSize)
    : m_Count(_vertexCount)
    , m_VerticiesSize(_verticiesSize)
{
    m_RhiBuffer.reset(rhi.CreateBuffer(_name));
    m_RhiBuffer
        ->SetSize(m_Count * m_VerticiesSize)
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex | RhiBuffer::BufferUsageFlagBits::TransferDst);
}

PC_CORE::VertexBuffer::VertexBuffer(PC_CORE::Rhi& rhi, const std::string& _name, const size_t _sizeInBytes)
{
    m_RhiBuffer.reset(rhi.CreateBuffer(_name));
    m_RhiBuffer
        ->SetSize(_sizeInBytes)
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Vertex | RhiBuffer::BufferUsageFlagBits::TransferDst);
}
