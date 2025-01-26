#pragma once
#include <memory>

#include "core_header.hpp"
#include "rhi_render_pass.hpp"


BEGIN_PCCORE
   class SwapChain 
{
public:
    PC_CORE_API SwapChain() = default;
    
    PC_CORE_API virtual ~SwapChain() = default;

    PC_CORE_API std::shared_ptr<RhiRenderPass> GetSwapChainRenderPass() const;
    
protected:
    std::shared_ptr<RhiRenderPass> m_SwapChainRenderPass;
};

END_PCCORE