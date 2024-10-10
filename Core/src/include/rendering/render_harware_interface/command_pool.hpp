#pragma once

#include "rhi_typedef.h"

BEGIN_PCCORE

class CommandPool
{
public:

    CommandPool(CommandPool&& other) noexcept;

    CommandPool operator=(CommandPool&& other) noexcept;
    
    CommandPool(const CommandPoolCreateInfo& _commandPoolCreateInfo);
    
    CommandPool() = default;

    ~CommandPool();

private:
    ObjectHandle m_hCommandPool = INVALID_HANDLE;
};

END_PCCORE