#pragma once

#include "core_header.hpp"
#include "gpu_buffer_handle.hpp"

BEGIN_PCCORE
    class GpuAllocator
{
public:

    bool CreateVulkanBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<GpuBufferHandle>* _vulkanBufferPtr);
        
    bool DestroyBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<GpuBufferHandle>* _vulkanBufferPtr);
private:
    
};

END_PCCORE