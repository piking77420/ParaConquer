#include "LowRenderer/SwapChain.hpp"

#include "LowRenderer/Rhi.hpp"

using namespace PC_CORE;

RhiSwapChain::RhiSwapChain(Rhi& _Rhi, const std::string& _Name, uint32_t _Widht, uint32_t _Height)
    : RhiObject(_Rhi, _Name)
    , m_SwapChainWidth(_Widht)
    , m_SwapChainHeight(_Height)
{

}

RhiSwapChain::RhiSwapChain(Rhi& _Rhi, std::string&& _Name, uint32_t _Widht, uint32_t _Height)
    : RhiObject(_Rhi, std::move(_Name))
    , m_SwapChainWidth(_Widht)
    , m_SwapChainHeight(_Height)
{

}

std::shared_ptr<RhiRenderPass> RhiSwapChain::GetSwapChainRenderPass() const
{
    return m_SwapChainRenderPass;
}
