#pragma once

#include "app.hpp"
#include "rhi_typedef.h"

BEGIN_PCCORE
  
class DescriptorPool
{
public:

    DescriptorPool() = default;
    
    DescriptorPool(DesciptorPoolSize* desciptorPoolSize, uint32_t descriptorCount, uint32_t maxSets);

    ~DescriptorPool();

private:
    DescriptorPoolHandle m_DescriptorPool = nullptr;
};

END_PCCORE