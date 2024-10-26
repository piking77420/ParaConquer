#pragma once

#include "rhi_typedef.h"

BEGIN_PCCORE

class DescriptorSet
{
public:
    DescriptorSet(DescriptorSet&& _other) noexcept;

    DescriptorSet& operator=(DescriptorSet&& _other) noexcept;

    DescriptorSet() = default;
    
    ~DescriptorSet() = default;

    DescriptorSetHandle handle = nullptr;
};

static_assert(sizeof(DescriptorSet) == 8, "Size of DescriptorSet is not 8 bytes");

END_PCCORE