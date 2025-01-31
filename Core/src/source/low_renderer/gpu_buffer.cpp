#include "low_renderer/gpu_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::GpuBuffer::~GpuBuffer()
{
    if (handle == nullptr)
        return;
    
    if (!Rhi::GetRhiContext()->gpuAllocator->DestroyBuffer(&handle))
    {
        PC_LOGERROR("Failed to destroy GPU buffer");
    }
}
