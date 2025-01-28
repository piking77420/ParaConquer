#pragma once

#include "core_header.hpp"
#include "gpu_buffer_handle.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE

class GpuAllocator
{
public:
    virtual bool CreateVulkanBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<GpuBufferHandle>* _bufferptr) = 0;
        
    virtual bool DestroyBuffer(std::shared_ptr<PC_CORE::GpuBufferHandle>* _bufferptr) = 0;
private:
    
};

END_PCCORE