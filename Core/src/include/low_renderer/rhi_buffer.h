#pragma once

#include "rhi_resource.hpp"

BEGIN_PCCORE

class RhiBuffer : public RhiResource
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(RhiBuffer)
    
    PC_CORE_API RhiBuffer() = default;

    PC_CORE_API virtual ~RhiBuffer() = default;

    virtual void MapData(void** _ptr)
    {
        
    }
    virtual void UnmapData()
    {
        
    }

private:
    
};

END_PCCORE