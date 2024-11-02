#include "rendering/render_pass.hpp"

PC_CORE::RenderPassHandle PC_CORE::RenderPass::GetHandle() const
{
    return m_RenderPassHandle;
}

PC_CORE::RenderPass::RenderPass(RenderPass&& _other) noexcept
{
    m_RenderPassHandle = _other.m_RenderPassHandle;
    _other.m_RenderPassHandle = nullptr;
}

PC_CORE::RenderPass::RenderPass(const RenderPassCreateInfo& _renderPassCreateInfo)
{
    
}
