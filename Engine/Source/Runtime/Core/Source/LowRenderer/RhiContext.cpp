#include "LowRenderer/RhiContext.hpp"

#include "LowRenderer/Rhi.hpp"

namespace PC_CORE 
{

    RhiContext::RhiContext(Rhi& _Rhi)
        : m_Rhi(_Rhi)
    {
    }

    RHI::ResourceUpdateBranch* RhiContext::ResourceUpdateBranch()
    {
        m_ResourceUpdate.push_back(RHI::ResourceUpdateBranch{});
        return &m_ResourceUpdate.back();
    }

    bool RhiContext::PendingTransferOperation() const
    {
        return m_PendingTransferOperation;
    }
}
