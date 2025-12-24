#include "LowRenderer/SwapChain.hpp"

#include "LowRenderer/Rhi.hpp"

using namespace PC_CORE;

RhiSwapChain::RhiSwapChain(Rhi& _Rhi, uint32_t _Widht, uint32_t _Height)
    : RhiObjectT(_Rhi)
    , m_SwapChainWidth(_Widht)
    , m_SwapChainHeight(_Height)
{

}

std::shared_ptr<RhiRenderPass> RhiSwapChain::GetSwapChainRenderPass() const
{
    return m_SwapChainRenderPass;
}
