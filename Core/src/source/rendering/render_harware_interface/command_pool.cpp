#include "rendering/render_harware_interface/command_pool.hpp"

PC_CORE::CommandPool::CommandPool(CommandPool&& other) noexcept
{
    m_hCommandPool = other.m_hCommandPool;
    other.m_hCommandPool = nullptr;
}

PC_CORE::CommandPool PC_CORE::CommandPool::operator=(CommandPool&& other) noexcept
{
    m_hCommandPool = other.m_hCommandPool;
    other.m_hCommandPool = nullptr;
}

PC_CORE::CommandPool::CommandPool(const CommandPoolCreateInfo& _commandPoolCreateInfo)
{
    
}

PC_CORE::CommandPool::~CommandPool()
{
    if (m_hCommandPool != nullptr)
    {
        // delte comand pool
    }
    
}
