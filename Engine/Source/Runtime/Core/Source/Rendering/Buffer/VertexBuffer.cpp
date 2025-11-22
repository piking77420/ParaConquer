#include "Rendering/Buffer/VertexBuffer.hpp"

#include "LowRenderer/Rhi.hpp"



PC_CORE::VertexBuffer::VertexBuffer(PC_CORE::Rhi& rhi, const std::string& _name, size_t _vertexCount, size_t _verticiesSize, PC_CORE::RhiResource::MemoryUsage _memoryUsage)
    : m_Count(_vertexCount)
    , m_VerticiesSize(_verticiesSize)
{
    const RhiBuffer::RhiBufferDescriptor rhiBufferDescriptor = RhiBuffer::RhiBufferDescriptor
   {
       .SizeInByte = static_cast<uint32_t>(m_Count * m_VerticiesSize),
       .Usage = static_cast<RhiBuffer::BufferUsage>(RhiBuffer::BufferUsage::Vertex | RhiBuffer::BufferUsage::TransferDst),
    };
    
    m_RhiBuffer.reset(rhi.CreateBuffer(_name, rhiBufferDescriptor, _memoryUsage));
}

PC_CORE::VertexBuffer::VertexBuffer(PC_CORE::Rhi& rhi, const std::string& _name, const size_t _sizeInBytes,
    PC_CORE::RhiResource::MemoryUsage _memoryUsage)
{
    
    const RhiBuffer::RhiBufferDescriptor rhiBufferDescriptor = RhiBuffer::RhiBufferDescriptor
  {
      .SizeInByte = static_cast<uint32_t>(_sizeInBytes),
      .Usage = static_cast<RhiBuffer::BufferUsage>(RhiBuffer::BufferUsage::Vertex),
   };
    
    m_RhiBuffer.reset(rhi.CreateBuffer(_name, rhiBufferDescriptor, _memoryUsage));
}
