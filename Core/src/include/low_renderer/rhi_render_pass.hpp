#pragma once

#include "rhi_typedef.h"

BEGIN_PCCORE

class RhiRenderPass
{
public:
    
    PC_CORE_API RhiRenderPass(PC_CORE::RHIFormat colorFormat, PC_CORE::RHIFormat depthFormat) {}
    
    PC_CORE_API RhiRenderPass() = default;

    PC_CORE_API virtual ~RhiRenderPass() = default;
    
protected:
};

END_PCCORE
