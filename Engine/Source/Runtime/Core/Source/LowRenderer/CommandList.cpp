#include "LowRenderer/CommandList.hpp"

PC_CORE::CommandList::CommandList(Rhi& _Rhi)
    : RhiObjectT(_Rhi)
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
