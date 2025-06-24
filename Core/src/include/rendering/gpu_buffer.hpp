#pragma once

#include <array>

#include "low_renderer/gpu_resource.hpp"

BEGIN_PCCORE

class GpuBuffer
{
public:

    PC_CORE_API GpuBuffer();
    
    PC_CORE_API virtual ~GpuBuffer();
};

END_PCCORE