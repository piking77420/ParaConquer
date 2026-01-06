#include "LowRenderer/SwapChain.hpp"

#include "LowRenderer/Rhi.hpp"

using namespace PC_CORE;

RhiSwapChain::RhiSwapChain(Rhi& _Rhi)
    : RhiObjectT(_Rhi)
{

}

std::shared_ptr<RhiRenderPass> RhiSwapChain::GetSwapChainRenderPass() const
{
    return m_SwapChainRenderPass;
}
