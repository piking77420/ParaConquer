#pragma once

#include "core_header.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE

class RhiResource
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(RhiResource)
    
    PC_CORE_API virtual const void* GetNativeHandle() const  = 0;
    
    PC_CORE_API RhiResource() = default;

    PC_CORE_API virtual ~RhiResource() = default;
    
};



END_PCCORE
