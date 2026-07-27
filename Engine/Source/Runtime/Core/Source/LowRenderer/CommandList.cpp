#include "LowRenderer/CommandList.hpp"

namespace PC_CORE 
{
    PC_CORE::CommandList::CommandList(Rhi& _Rhi)
        : RhiObjectT(_Rhi)
    {
    }

    void CommandList::BeginRecordCommands()
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rhi);
    }

    void CommandList::EndRecordCommands()
    {
        
    }

    void PC_CORE::CommandList::BeginRenderPass(const BeginRenderPassInfo& _beginRenderPassInfo)
    {
        m_RecordState = {};
        m_RecordState.RecordedRenderPassType = RecordRenderPassType::Graphic;
    }

    void CommandList::BeginComputePasss()
    {
        m_RecordState = {};
        m_RecordState.RecordedRenderPassType = RecordRenderPassType::Compute;
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

        const bool changed = m_RecordState.lastDrawBuffersState != _DrawBuffers;
        if (changed)
            m_RecordState.lastDrawBuffersState = _DrawBuffers;

        return changed;
    }

    bool CommandList::IsInRenderPass(RecordRenderPassType Type) const
    {
        return m_RecordState.RecordedRenderPassType == Type;
    }

} // namespace PC_CORE

