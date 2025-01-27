#include "low_renderer/rhi_context.hpp"



PC_CORE::RhiContext::~RhiContext()
{

}

void PC_CORE::RhiContext::WaitIdle()
{
	m_CurrentContext->WaitIdleInstance();
}

