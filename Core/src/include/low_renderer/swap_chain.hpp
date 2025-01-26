#pragma once
#include <memory>
#include <vector>

#include "core_header.hpp"
#include "frame_buffer.hpp"
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

    std::vector<std::shared_ptr<PC_CORE::FrameBuffer>> m_SwapChainFramebuffers;

    size_t m_SwapChainImageIndex = 0;
};

END_PCCORE