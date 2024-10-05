#pragma once

#include "rhi_typedef.h"

BEGIN_PCCORE



class CommandPool
{

	CommandPool(const CommandPoolCreateInfo& commandPoolCreateInfo);

	CommandPool() = default;

	~CommandPool();

private:
	void* m_CommandPoolHandle = nullptr;
};

END_PCCORE