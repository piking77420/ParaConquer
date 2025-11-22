#include "LowRenderer/RhiBuffer.h"

PC_CORE::RhiBuffer::RhiBuffer(Rhi& _Rhi, const std::string& _name, const RhiBufferDescriptor& _rhiBufferDescriptor, MemoryUsage _memoryUsage)
    : RhiResource(_Rhi, _name, _memoryUsage)
    , m_RhiBufferDescriptor(_rhiBufferDescriptor)
{
}

PC_CORE::RhiBuffer::RhiBuffer(Rhi& _Rhi, std::string&& _name, const RhiBufferDescriptor& _rhiBufferDescriptor, MemoryUsage _memoryUsage)
    : RhiResource(_Rhi, _name, _memoryUsage)
    , m_RhiBufferDescriptor(std::move(_rhiBufferDescriptor))
{
    
}