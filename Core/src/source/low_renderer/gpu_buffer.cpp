#include "low_renderer/gpu_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE_API PC_CORE::GpuBuffer::GpuBuffer(GpuBuffer&& _other) noexcept
{
    

    for (size_t i = 0; i < bufferHandles.size(); ++i)
    {
        bufferHandles[i] = _other.bufferHandles[i];
        _other.bufferHandles[i] = nullptr;
    }

}
    
PC_CORE_API PC_CORE::GpuBuffer& PC_CORE::GpuBuffer::operator=(GpuBuffer&& _other) noexcept
{
    for (size_t i = 0; i < bufferHandles.size(); ++i)
    {
        bufferHandles[i] = _other.bufferHandles[i];
        _other.bufferHandles[i] = nullptr;
    }

    return *this;
}

PC_CORE::GpuBuffer::~GpuBuffer()
{

    for (auto& sptrHandle : bufferHandles)
    {
        if (sptrHandle == nullptr)
            continue;
    
        if (!Rhi::GetRhiContext()->gpuAllocator->DestroyBuffer(&sptrHandle))
        {
            PC_LOGERROR("Failed to destroy GPU buffer");
        }
        sptrHandle = nullptr;
    }
    
   
}
