#pragma once

#include "core_header.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE
    
class CommandBuffer
{
public:

    CommandBuffer(const CommandBufferCreateInfo& _commandBufferCreateInfo);
    
    CommandBuffer();

    ~CommandBuffer();
    
private:
    ObjectHandle m_Handle;
};

END_PCCORE