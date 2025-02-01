#include "low_renderer/rhi_context.hpp"


PC_CORE::RhiContext::RhiContext(const RhiContextCreateInfo& rhiContextCreateInfo)
{
	m_CurrentContext = this;
};


void PC_CORE::RhiContext::WaitIdle()
{
	m_CurrentContext->WaitIdleInstance();
}


