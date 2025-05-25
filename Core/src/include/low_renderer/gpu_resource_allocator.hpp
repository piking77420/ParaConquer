#pragma once

#include <unordered_map>

#include "core_header.hpp"
#include "gpu_resource.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE

class GpuResourceAllocator
{
public:
    virtual bool CreateGPUBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<GPUResource>* _bufferptr) = 0;
        
    virtual bool DestroyBuffer(PC_CORE::GPUResource* _bufferptr) = 0;
    
    virtual bool MapBuffer(const std::shared_ptr<PC_CORE::GPUResource>& _bufferptr, void** _mapPtr) = 0;
    
    virtual bool UnMapBuffer(const std::shared_ptr<PC_CORE::GPUResource>& _bufferptr) = 0;

    virtual bool CreateTexture(const PC_CORE::CreateTextureInfo& _createTextureInfo, std::shared_ptr<GPUResource>* _texturePtr) = 0;

    virtual bool DestroyImage(GPUResource* _textureHandle) = 0;

};

END_PCCORE