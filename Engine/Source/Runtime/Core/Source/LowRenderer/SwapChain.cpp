#include "LowRenderer/SwapChain.hpp"

#include "LowRenderer/Rhi.hpp"

using namespace PC_CORE;

SwapChain::SwapChain(uint32_t _widht, uint32_t _height) :
    m_SwapChainWidth(_widht), m_SwapChainHeight(_height)
{
}

std::shared_ptr<RhiRenderPass> SwapChain::GetSwapChainRenderPass() const
{
    return m_SwapChainRenderPass;
}
