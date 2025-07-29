#include "low_renderer/swap_chain.hpp"

#include "low_renderer/rhi.hpp"

using namespace PC_CORE;

SwapChain::SwapChain(uint32_t _widht, uint32_t _height) :
    m_SwapChainWidth(_widht), m_SwapChainHeight(_height)
{
    const PC_CORE::CommandListCreateInfo createInfo =
    {
        .commandPoolFamily = CommandPoolFamily::Graphics,
        .commandBufferType = CommandBufferType::Primary
    };

    m_CommandList = PC_CORE::Rhi::CreateCommandList(createInfo);
}

 std::shared_ptr<RhiRenderPass> SwapChain::GetSwapChainRenderPass() const
{
    return m_SwapChainRenderPass;
}

