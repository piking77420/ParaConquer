#pragma once

#include <memory>

#include "rhi_typedef.h"

BEGIN_PCCORE

using GPUHandleID = uint32_t;
constexpr GPUHandleID GPU_INVALID_ID = std::numeric_limits<GPUHandleID>::max();
constexpr GPUHandleID MAX_ID = 4096;

class GPUResource
{
public:
	GPUResource()
	{
		Clear();
	};

   PC_CORE_API virtual ~GPUResource() = default;

   PC_CORE_API virtual void Clear() {};

};

END_PCCORE