#include "low_renderer/gpu_buffer.hpp"

#include "low_renderer/rhi.hpp"

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
