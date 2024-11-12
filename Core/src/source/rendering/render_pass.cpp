#include "rendering/render_pass.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::RenderPassHandle PC_CORE::RenderPass::GetHandle() const
{
    return m_RenderPassHandle;
}

PC_CORE::RenderPass& PC_CORE::RenderPass::operator=(RenderPass&& _other) noexcept
{
    Destroy();
    
    m_RenderPassHandle = _other.m_RenderPassHandle;
    _other.m_RenderPassHandle = nullptr;

    return *this;
}

PC_CORE::RenderPass::RenderPass(RenderPass&& _other) noexcept
{
    Destroy();
    
    m_RenderPassHandle = _other.m_RenderPassHandle;
    _other.m_RenderPassHandle = nullptr;
}

PC_CORE::RenderPass:: RenderPass(const RenderPassCreateInfo& _renderPassCreateInfo) 
{
    m_RenderPassHandle = RHI::GetInstance().CreateRenderPass(_renderPassCreateInfo);
}

PC_CORE::RenderPass::~RenderPass()
{
    if (m_RenderPassHandle != nullptr)
    {
        RHI::GetInstance().DestroyRenderPass(m_RenderPassHandle);
        m_RenderPassHandle = nullptr;
    }
}

void PC_CORE::RenderPass::Begin(CommandBuffer _commandBuffer, const BeginRenderPassInfo& _renderPassBeginInfo)
{
    RHI::GetInstance().BeginRenderPass(_commandBuffer, m_RenderPassHandle, _renderPassBeginInfo);
}

void PC_CORE::RenderPass::End(CommandBuffer _commandBuffer)
{
    RHI::GetInstance().EndRenderPass(_commandBuffer);
}

void PC_CORE::RenderPass::Destroy()
{
    if (m_RenderPassHandle != NULL_HANDLE)
    {
        RHI::GetInstance().DestroyRenderPass(m_RenderPassHandle);
        m_RenderPassHandle = NULL_HANDLE;
    }
}
