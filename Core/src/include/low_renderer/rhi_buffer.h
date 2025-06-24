#pragma once

#include "rhi_typedef.h"

BEGIN_PCCORE

class RhiBuffer
{
public:

    PC_CORE_API RhiBuffer(const GPUBufferCreateInfo& _createInfo);
    
    PC_CORE_API RhiBuffer() = default;

    PC_CORE_API virtual ~RhiBuffer() = default;

private:
    
};

END_PCCORE