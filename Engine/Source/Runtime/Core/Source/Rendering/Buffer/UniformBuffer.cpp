#include "Rendering/Buffer/UniformBuffer.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::UniformBuffer::UniformBuffer(PC_CORE::Rhi& rhi, const std::string& _name, size_t _size, PC_CORE::RhiResource::MemoryUsage _memoryUsage)
{
    
    const RhiBuffer::RhiBufferDescriptor rhiBufferDescriptor = RhiBuffer::RhiBufferDescriptor
     {
         .SizeInByte = static_cast<uint32_t>(_size),
         .Usage = static_cast<RhiBuffer::BufferUsage>(RhiBuffer::BufferUsage::Uniform),
     };
    
    m_RhiBuffer.reset(rhi.CreateBuffer(_name, rhiBufferDescriptor, _memoryUsage));
}