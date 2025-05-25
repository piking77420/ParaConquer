#include "low_renderer/shader_storage.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::ShaderStorage::ShaderStorage(void* _data, size_t _size, MemoryUsageType MemoryUsageType)
{
    const GPUBufferCreateInfo info =
        {
        .data = _data,
        .dataSize = _size,
        .usage = BufferUsage::ShaderStorageBuffer,
        .memoryUsage = MemoryUsageType
        };

    for (auto& it : bufferHandles)
    {
        it = Rhi::CreateBuffer(info);
    }
}

PC_CORE::ShaderStorage::ShaderStorage(size_t _size, MemoryUsageType MemoryUsageType)
{
    
}

PC_CORE::ShaderStorage::~ShaderStorage()
{
    
}
