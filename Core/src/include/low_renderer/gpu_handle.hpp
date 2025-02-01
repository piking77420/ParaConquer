#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

class GpuHandle
{
public:
    PC_CORE_API GpuHandle() = default;

   PC_CORE_API  virtual ~GpuHandle() = default;
};

END_PCCORE