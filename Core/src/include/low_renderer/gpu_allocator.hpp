#pragma once

#include "core_header.hpp"
#include "gpu_buffer_handle.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE

class GpuAllocator
{
public:
    virtual bool CreateGPUBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<GpuBufferHandle>* _bufferptr) = 0;
        
    virtual bool DestroyBuffer(std::shared_ptr<PC_CORE::GpuBufferHandle>* _bufferptr) = 0;
    
    virtual bool MapBuffer(const std::shared_ptr<PC_CORE::GpuBufferHandle>& _bufferptr, void** _mapPtr) = 0;
    
    virtual bool UnMapBuffer(const std::shared_ptr<PC_CORE::GpuBufferHandle>& _bufferptr) = 0;

    
private:
    
};

END_PCCORE