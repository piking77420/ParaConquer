#pragma once

#include "core_header.hpp"
#include "low_renderer/rhi_render_pass.hpp"
#include "rendering/renderer.hpp"

BEGIN_PCCORE
    class RenderPass
{
public:
    PC_CORE_API RenderPass();
    
    PC_CORE_API virtual ~RenderPass() = default;

    PC_CORE_API virtual void Build() = 0;

    PC_CORE_API virtual void Execute() = 0;
    
protected:
    std::shared_ptr<RhiRenderPass> m_renderPassHandle;

   // RenderResources* m_RenderResources = nullptr;
};

END_PCCORE