#include "LowRenderer/CommandList.hpp"

namespace PC_CORE 
{
    PC_CORE::CommandList::CommandList(Rhi& _Rhi)
        : RhiObjectT(_Rhi)
    {
    }

    void CommandList::BeginRecordCommands()
    {
        m_LastDrawBuffersState = {};
    }

    void CommandList::EndRecordCommands()
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

    bool CommandList::DrawBufferStateChanged(const DrawBuffers& _DrawBuffers)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rhi);

        const bool changed = m_LastDrawBuffersState != _DrawBuffers;
        if (changed)
            m_LastDrawBuffersState = _DrawBuffers;

        return changed;
    }

} // namespace PC_CORE

