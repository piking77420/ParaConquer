#pragma once

#include "RhiResource.hpp"


BEGIN_PCCORE

struct RhiFenceCreateInfo
{
	bool signaled;
};

class RhiFence : public RhiResource
{
public:

	PC_CORE_API const void* GetNativeHandle() const = 0;

	PC_CORE_API virtual void WaitForFence(bool _waitAll, uint32_t _time = UINT64_MAX) = 0;

	PC_CORE_API virtual void Reset() = 0;

	DEFAULT_COPY_MOVE_OPERATIONS(RhiFence);

	PC_CORE_API RhiFence() = default;

	PC_CORE_API virtual ~RhiFence() = default;

private:

};

END_PCCORE