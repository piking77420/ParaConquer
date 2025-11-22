#include "Rendering/Buffer/IndexBuffer.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiBuffer.h"


PC_CORE::IndexBuffer::IndexBuffer(Rhi& rhi, const std::string& _name, size_t _indexCount,  RhiBuffer::IndexFormat _indexFormat,
                                  PC_CORE::RhiResource::MemoryUsage _memoryUsage)
      : m_RhiBufferFormat(_indexFormat)
      , m_IndiciesCount(_indexCount)
{
   
   const RhiBuffer::RhiBufferDescriptor rhiBufferDescriptor = RhiBuffer::RhiBufferDescriptor
   {
      .SizeInByte = static_cast<uint32_t>(_indexCount * static_cast<size_t>(_indexFormat)),
      .Usage = static_cast<RhiBuffer::BufferUsage>(RhiBuffer::BufferUsage::Index | RhiBuffer::BufferUsage::TransferDst),
   };
   
   m_RhiBuffer.reset(rhi.CreateBuffer(_name, rhiBufferDescriptor, _memoryUsage));
}
