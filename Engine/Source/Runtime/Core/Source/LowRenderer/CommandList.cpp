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

        m_LastDrawBuffersState = {};
        m_LastBindProgram = nullptr;
    }

    void CommandList::EndRecordCommands()
    {
        
    }

    void PC_CORE::CommandList::BeginRenderPass(const BeginRenderPassInfo& _beginRenderPassInfo)
    {
        m_LastBindProgram = nullptr;
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

    const RhiShaderProgram& CommandList::GetLastBindProgram() const
    {
        assert(m_LastBindProgram != nullptr && "Forgot to bind a ShaderProgram ? ");

        return *m_LastBindProgram;
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

    void CommandList::BindProgram(const RhiShaderProgram& _RhiShaderProgram)
    {
        m_LastBindProgram = &_RhiShaderProgram;
    }


} // namespace PC_CORE

