#pragma once

#include "core_header.hpp"
#include "gpu_handle.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE

class GpuAllocator
{
public:
    virtual bool CreateGPUBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<GpuHandle>* _bufferptr) = 0;
        
    virtual bool DestroyBuffer(std::shared_ptr<PC_CORE::GpuHandle>* _bufferptr) = 0;
    
    virtual bool MapBuffer(const std::shared_ptr<PC_CORE::GpuHandle>& _bufferptr, void** _mapPtr) = 0;
    
    virtual bool UnMapBuffer(const std::shared_ptr<PC_CORE::GpuHandle>& _bufferptr) = 0;

    virtual bool CreateTexture(const PC_CORE::CreateTextureInfo& _createTextureInfo, std::shared_ptr<GpuHandle>* _texturePtr) = 0;

    virtual bool DestroyTexture(std::shared_ptr<GpuHandle>* _textureHandle) = 0;

    
private:
    
};

END_PCCORE