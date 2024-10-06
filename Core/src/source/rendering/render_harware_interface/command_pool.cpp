#include "rendering/render_harware_interface/command_pool.hpp"
#include "rendering/render_harware_interface/RHI.hpp"

using namespace PC_CORE;

CommandPool::~CommandPool()
{
	if (m_Handle != nullptr)
	{
		RHI::GetInstance()->DestroyCommandPool(m_Handle);
		m_Handle = nullptr;
	}
}

void CommandPool::AllocCommandBuffers(CommandBufferHandle* _commandBuffer, uint32_t _numCommandBuffers)
{
	RHI::GetInstance()->AllocCommandBuffers(m_Handle, _commandBuffer, _numCommandBuffers);
}

void CommandPool::FreeCommandBuffers(CommandBufferHandle* _commandBuffer, uint32_t _numCommandBuffers)
{
	RHI::GetInstance()->FreeCommandBuffers(m_Handle, _commandBuffer, _numCommandBuffers);
}

CommandPool& CommandPool::operator=(CommandPool&& _commandPool) noexcept
{
	 this->m_Handle = _commandPool.m_Handle;
	_commandPool.m_Handle = nullptr;
	
	return *this;
}

CommandPool::CommandPool(CommandPool&& _commandPool) noexcept
{
	this->m_Handle = _commandPool.m_Handle;
	_commandPool.m_Handle = nullptr;
	
}


CommandPool::CommandPool(const CommandPoolCreateInfo& commandPoolCreateInfo)
{
	RHI::GetInstance()->CreateCommandPool(commandPoolCreateInfo, &m_Handle);
}
