#include "low_renderer/gpu_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::GpuBuffer::~GpuBuffer()
{
    if (m_GpuHandle == nullptr)
        return;
    
    if (!Rhi::GetRhiContext()->gpuAllocator->DestroyBuffer(&m_GpuHandle))
    {
        PC_LOGERROR("Failed to destroy GPU buffer");
    }
}
