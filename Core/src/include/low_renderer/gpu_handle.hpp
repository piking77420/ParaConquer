#pragma once

#include <memory>

#include "rhi_typedef.h"

BEGIN_PCCORE

class GpuHandle
{
public:
    PC_CORE_API GpuHandle() = default;

   PC_CORE_API  virtual ~GpuHandle() = default;
};

END_PCCORE