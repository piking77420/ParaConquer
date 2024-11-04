#include "rendering/render_pass.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::RenderPassHandle PC_CORE::RenderPass::GetHandle() const
{
    return m_RenderPassHandle;
}

PC_CORE::RenderPass::RenderPass(RenderPass&& _other) noexcept
{
    Destroy();
    
    m_RenderPassHandle = _other.m_RenderPassHandle;
    _other.m_RenderPassHandle = nullptr;
}

PC_CORE::RenderPass::RenderPass(const RenderPassCreateInfo& _renderPassCreateInfo) 
{
    
}

void PC_CORE::RenderPass::Build()
{
}

void PC_CORE::RenderPass::Destroy()
{
    if (m_RenderPassHandle != NULL_HANDLE)
    {
        RHI::GetInstance().DestroyRenderPass(m_RenderPassHandle);
        m_RenderPassHandle = NULL_HANDLE;
    }
}
