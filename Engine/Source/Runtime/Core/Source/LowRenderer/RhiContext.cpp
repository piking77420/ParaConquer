#include "LowRenderer/RhiContext.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::RhiContext::RhiContext(const RhiContextCreateInfo& rhiContextCreateInfo)
{
    m_CurrentContext = this;
};


void PC_CORE::RhiContext::WaitIdle()
{
    m_CurrentContext->WaitIdleInstance();
}
