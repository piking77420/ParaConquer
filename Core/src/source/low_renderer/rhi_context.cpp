#include "low_renderer/rhi_context.hpp"


void PC_CORE::RhiContext::WaitIdle()
{
	m_CurrentContext->WaitIdleInstance();
}

