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
        std::scoped_lock _(m_ResourceUpdateLock);

        m_ResourceUpdate.push_back(std::make_unique<RHI::ResourceUpdateBranch>());
        return m_ResourceUpdate.back().get();
    }

    RHI::ResourceUpdateBranch* RhiContext::ResourceUpdateBranch_AssumeLock()
    {
        m_ResourceUpdate.push_back(std::make_unique<RHI::ResourceUpdateBranch>());
        return m_ResourceUpdate.back().get();
    }

    bool RhiContext::PendingTransferOperation() const
    {
        return m_PendingTransferOperation;
    }
}
