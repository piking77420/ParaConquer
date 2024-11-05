#pragma once

#include "rhi_typedef.h"
#include "descriptor_set.hpp"
#include "resources/descriptor_set_layout.h"

BEGIN_PCCORE
  
class DescriptorPool
{
public:

    DescriptorPool(DescriptorPool&& _other) noexcept;

    DescriptorPool& operator=(DescriptorPool&& _other) noexcept;
    
    DescriptorPool() = default;

    void AllocDescriptorSet(DescriptorSetHandle* descriptorSet, uint32_t descriptorSetCount, const DescriptorSetLayout& descriptorSetLayoutayout);
    
    DescriptorPool(const DescriptorPoolSize* desciptorPoolSize, uint32_t descriptorCount, uint32_t maxSets);

    ~DescriptorPool();

private:
    DescriptorPoolHandle m_Handle = nullptr;
};

END_PCCORE