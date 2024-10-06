#pragma once

#include "rhi_typedef.h"

BEGIN_PCCORE


class CommandPool
{
public:	
	void AllocCommandBuffers(CommandBufferHandle* _commandBuffer, uint32_t _numCommandBuffers);

	void FreeCommandBuffers(CommandBufferHandle* _commandBuffer, uint32_t _numCommandBuffers);

	CommandPool& operator=(CommandPool&& _commandPool) noexcept;

	CommandPool(CommandPool&& _commandPool) noexcept;
	
	CommandPool(const CommandPoolCreateInfo& commandPoolCreateInfo);

	CommandPool() = default;

	~CommandPool();

private:
	CommandPoolHandle m_Handle = nullptr;
};

END_PCCORE