#include "rendering/render_harware_interface/command_pool.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::CommandPool::CommandPool(CommandPool&& other) noexcept
{
    m_hCommandPool = other.m_hCommandPool;
    other.m_hCommandPool = nullptr;
}

PC_CORE::CommandPool& PC_CORE::CommandPool::operator=(CommandPool&& other) noexcept
{
    m_hCommandPool = other.m_hCommandPool;
    other.m_hCommandPool = nullptr;
    
    return *this;
}

PC_CORE::CommandPool::CommandPool(const CommandPoolCreateInfo& _commandPoolCreateInfo)
{
    RHI::GetInstance().CreateCommandPool(_commandPoolCreateInfo, &m_hCommandPool);
}

PC_CORE::CommandPool::~CommandPool()
{
    if (m_hCommandPool != nullptr)
    {
        RHI::GetInstance().DestroyCommandPool(m_hCommandPool);
        m_hCommandPool = nullptr;
    }
}

PC_CORE::CommandPoolHandle PC_CORE::CommandPool::GetHandle() const
{
    return m_hCommandPool;
}

void PC_CORE::CommandPool::AllocCommandBuffer(const PC_CORE::CommandBufferCreateInfo& _commandBufferCreateInfo)
{
    RHI::GetInstance().AllocCommandBuffers(m_hCommandPool, _commandBufferCreateInfo);
}

void PC_CORE::CommandPool::DestroyCommandBuffer(CommandBuffer* _commandBuffer, uint32_t _commandBufferCount)
{
    RHI::GetInstance().FreeCommandBuffer(m_hCommandPool, _commandBuffer, _commandBufferCount);
}
