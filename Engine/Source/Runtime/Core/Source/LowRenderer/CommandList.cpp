#include "LowRenderer/CommandList.hpp"


PC_CORE_API PC_CORE::CommandList::CommandList(const CommandListCreateInfo& _commandListCreateInfo) : m_CommandPoolFamily(_commandListCreateInfo.commandPoolFamily)
{
	
}

void PC_CORE::CommandList::RecordFetchCommand(std::function<void(CommandList*)> _fectFunction)
{
	m_FecthCommands.push_back(_fectFunction);
}

void PC_CORE::CommandList::ExecuteExternalCommand()
{
	for (auto& command : m_FecthCommands)
		command(this);
}
