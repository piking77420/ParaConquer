#pragma once

#include <memory>

#include "rhi_typedef.h"

BEGIN_PCCORE

using GPUHandleID = uint32_t;
constexpr GPUHandleID GPU_INVALID_ID = std::numeric_limits<GPUHandleID>::max();
constexpr GPUHandleID MAX_ID = 4096;

struct GPUResource
{
   PC_CORE_API virtual ~GPUResource() = default;
};

END_PCCORE