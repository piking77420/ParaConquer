#pragma once

#include "RhiObject.hpp"


BEGIN_PCCORE
    struct RhiFenceCreateInfo
    {
        bool signaled;
    };

    class RhiFence : public RhiObject
    {
    public:
        PC_CORE_API const void* GetFrameNativeHandle(size_t _frameIndex) const override = 0;

        PC_CORE_API virtual void WaitForFence(bool _waitAll, uint32_t _time = UINT64_MAX) = 0;

        PC_CORE_API virtual void Reset() = 0;

        DEFAULT_COPY_MOVE_OPERATIONS(RhiFence);

        PC_CORE_API explicit RhiFence(Rhi& _Rhi, const std::string& _name , const RhiFenceCreateInfo& _RhiFenceCreateInfo);

        PC_CORE_API explicit RhiFence(Rhi& _Rhin, std::string&& _name, const RhiFenceCreateInfo& _RhiFenceCreateInfo);

        PC_CORE_API RhiFence() = default;

        PC_CORE_API ~RhiFence() override = default;

    protected: 
        RhiFenceCreateInfo m_RhiFenceCreateInfo{};
    };

END_PCCORE
