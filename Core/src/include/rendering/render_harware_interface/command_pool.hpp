#pragma once

#include "command_buffer.h"
#include "rhi_typedef.h"

BEGIN_PCCORE

class CommandPool
{
public:

    CommandPool(CommandPool&& other) noexcept;

    CommandPool& operator=(CommandPool&& other) noexcept;
    
    CommandPool(const CommandPoolCreateInfo& _commandPoolCreateInfo);
    
    CommandPool() = default;

    ~CommandPool();

    CommandPoolHandle GetHandle() const;

    void AllocCommandBuffer(const PC_CORE::CommandBufferCreateInfo& _commandBufferCreateInfo);

    void DestroyCommandBuffer(CommandBuffer* _commandBuffer, uint32_t _commandBufferCount);

private:
    CommandPoolHandle m_hCommandPool = INVALID_HANDLE;
};

END_PCCORE