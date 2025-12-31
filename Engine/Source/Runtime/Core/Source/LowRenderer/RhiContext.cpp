#include "LowRenderer/RhiContext.hpp"

#include "LowRenderer/Rhi.hpp"

namespace PC_CORE 
{

    RhiContext::RhiContext(Rhi& _Rhi)
        : m_Rhi(_Rhi)
    {
    }
    
    void RhiContext::FetchResourceUpdate(RHI::ResourceUpdateBranch* _ResourceUpdateBranch)
    {
        m_ResourceUpdate.push_back(std::move(*_ResourceUpdateBranch));
        _ResourceUpdateBranch->Reset();
    }

    bool RhiContext::PendingExcutionResourceUpdate() const
    {
        return m_PendingExcutionResourceUpdate;
    }
}
