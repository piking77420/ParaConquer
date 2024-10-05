#include "rendering/render_harware_interface/command_pool.hpp"
#include "rendering/render_harware_interface/RHI.hpp"

using namespace PC_CORE;

CommandPool::~CommandPool()
{
	if (m_CommandPoolHandle != nullptr)
	{
		RHI::GetInstance()->DestroyCommandPool(&m_CommandPoolHandle);
	}
}

CommandPool::CommandPool(const CommandPoolCreateInfo& commandPoolCreateInfo)
{
	RHI::GetInstance()->CreateCommandPool(commandPoolCreateInfo, &m_CommandPoolHandle);
}