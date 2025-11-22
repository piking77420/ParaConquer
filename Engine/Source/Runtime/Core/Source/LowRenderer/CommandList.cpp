#include "LowRenderer/CommandList.hpp"


PC_CORE_API PC_CORE::CommandList::CommandList(const CommandListCreateInfo& _commandListCreateInfo) :
    m_CommandPoolFamily(_commandListCreateInfo.CommandPoolFamily)
{
}

void PC_CORE::CommandList::BeginRenderPass(const BeginRenderPassInfo& _beginRenderPassInfo)
{
}

void PC_CORE::CommandList::Barrier(GpuPipelineStageFlagBits _srcStageMask, GpuPipelineStageFlagBits _dstStageMask,
    const MemoryBarrier* _memoryBarrier, size_t _memoryBarrierCount, const BufferMemoryBarrier* _bufferMemoryBarrier,
    size_t _bufferMemoryBarrierCount, const ImageMemoryBarrier* _imageMemoryBarrier, size_t _imageMemoryBarrierCount)
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
