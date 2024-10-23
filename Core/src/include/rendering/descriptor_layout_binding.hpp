#pragma once

#include "render_harware_interface/rhi_typedef.h"

BEGIN_PCCORE

struct DescriptorLayoutBinding 
{
    uint32_t binding;
    uint32_t descriptorCount;
    DESCRIPTOR_TYPE descriptorType;
    std::vector<ShaderStageType> shaderStages;
    GPUBufferHandle pImmutableSanpler = nullptr;
};

END_PCCORE