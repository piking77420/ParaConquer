#include "low_renderer/rhi_context.hpp"

#include "low_renderer/rhi.hpp"

void PC_CORE::RhiContext::HandlePendingResourceFunction(CommandList* _commandList)
{
	for (auto& pendingFunc : m_PendingResourceFuncion)
	{
		pendingFunc(_commandList);
	}

	// then submit

}

PC_CORE::RhiContext::RhiContext(const RhiContextCreateInfo& rhiContextCreateInfo)
{
	m_CurrentContext = this;
};


void PC_CORE::RhiContext::WaitIdle()
{
	m_CurrentContext->WaitIdleInstance();
}


