#pragma once

#include "core_header.hpp"
#include "low_renderer/rhi_render_pass.hpp"
#include "rendering/renderer.hpp"

BEGIN_PCCORE

class RenderPass : public IGpuResource
{
public:

    PC_CORE_API virtual std::shared_ptr<RhiResource> GetRhiHandle() const
    {
        return m_RhiRenderPass;
    }

    PC_CORE_API RenderPass();
    
    PC_CORE_API virtual ~RenderPass() = default;
        
    
protected:
    std::shared_ptr<RhiRenderPass> m_RhiRenderPass;
};

END_PCCORE