#pragma once

#include "rendering/rendering_typedef.h"
#include "render_harware_interface/command_buffer.h"

BEGIN_PCCORE
    class RenderPass
{
public:
    PC_CORE_API RenderPassHandle GetHandle() const;

    PC_CORE_API RenderPass& operator=(RenderPass&& _other) noexcept;
    
    PC_CORE_API RenderPass(RenderPass&& _other) noexcept;
    
    PC_CORE_API RenderPass(const RenderPassCreateInfo& _renderPassCreateInfo);

    PC_CORE_API RenderPass() = default;

    PC_CORE_API ~RenderPass();

    PC_CORE_API void Begin(PC_CORE::CommandBuffer _commandBuffer, const BeginRenderPassInfo& _renderPassBeginInfo);

    PC_CORE_API void End(PC_CORE::CommandBuffer _commandBuffer);

protected:
    RenderPassHandle m_RenderPassHandle = nullptr;
    
    void Destroy();
};

END_PCCORE