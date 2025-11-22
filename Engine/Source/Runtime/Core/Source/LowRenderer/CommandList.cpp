#include "LowRenderer/CommandList.hpp"

PC_CORE::CommandList::CommandList(Rhi& _Rhi, const std::string& _name, const CommandListCreateInfo& _commandListCreateInfo)
    : RhiObject(_Rhi, _name)
    , m_CommandPoolFamily(_commandListCreateInfo.CommandPoolFamily)
    , m_CommandBufferType(_commandListCreateInfo.CommandBufferType)
{
}

PC_CORE::CommandList::CommandList(Rhi& _Rhi, std::string&& _name, const CommandListCreateInfo& _commandListCreateInfo)
    : RhiObject(_Rhi, std::move(_name))
    , m_CommandPoolFamily(_commandListCreateInfo.CommandPoolFamily)
    , m_CommandBufferType(_commandListCreateInfo.CommandBufferType)
{
}

void PC_CORE::CommandList::BeginRenderPass(const BeginRenderPassInfo& _beginRenderPassInfo)
{
}

void PC_CORE::CommandList::RecordFetchCommand(const std::function<void(CommandList*)>& _fectFunction)
{
    m_FetchCommands.push_back(_fectFunction);
}

void PC_CORE::CommandList::ExecuteExternalCommand()
{
    for (auto& command : m_FetchCommands)
        command(this);
}
