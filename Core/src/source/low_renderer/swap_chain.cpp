#include "low_renderer/swap_chain.hpp"


using namespace PC_CORE;

SwapChain::SwapChain(uint32_t _widht, uint32_t _height) :
    m_SwapChainWidth(_widht), m_SwapChainHeight(_height)
{
    
}

 std::shared_ptr<RhiRenderPass> SwapChain::GetSwapChainRenderPass() const
{
    return m_SwapChainRenderPass;
}


std::shared_ptr<FrameBuffer> SwapChain::GetFrameBuffer()
{
     return m_SwapChainFramebuffers[m_SwapChainImageIndex];
}
