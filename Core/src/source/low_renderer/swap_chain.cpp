#include "low_renderer/swap_chain.hpp"

using namespace PC_CORE;

 std::shared_ptr<RhiRenderPass> SwapChain::GetSwapChainRenderPass() const
{
    return m_SwapChainRenderPass;
}
