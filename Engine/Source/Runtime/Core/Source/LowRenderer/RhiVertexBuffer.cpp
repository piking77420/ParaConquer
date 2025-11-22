#include "LowRenderer/RhiVertexBuffer.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::RhiVertexBuffer::RhiVertexBuffer(size_t _sizeInByte, MemoryLocalisation _visibility,
                                          MemoryUsage _memoryUsage) : RhiBuffer(_sizeInByte, _visibility, _memoryUsage)
{
}
