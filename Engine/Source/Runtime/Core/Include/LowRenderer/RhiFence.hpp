#pragma once

#include "RhiObject.hpp"


BEGIN_PCCORE

class RhiFence : public RhiObjectT<RhiFence>
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiFence);

    PC_CORE_API explicit RhiFence(Rhi& _Rhi);

    PC_CORE_API RhiFence() = default;

    PC_CORE_API ~RhiFence() override = default;

    PC_CORE_API virtual void WaitForFence(bool _waitAll, uint32_t _time = UINT64_MAX) = 0;

    PC_CORE_API virtual void Reset() = 0;

    PC_CORE_API virtual bool Build() = 0;
        
    // Setter 

    RhiFence& SetSignaled(bool Signaled)
    {
        m_Signaled = Signaled;
        return *this;
    }

    // Getter

    bool GetSignaled() const
    {
        return m_Signaled;
    }

protected: 
    bool m_Signaled = false;
};

END_PCCORE
