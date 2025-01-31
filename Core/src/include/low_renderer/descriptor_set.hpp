#pragma once

#include <vector>

#include "core_header.hpp"

BEGIN_PCCORE

enum class ShaderProgramDescriptorType
{
    UniformBuffer,
    Count,
};

struct ShaderProgramDescriptorWrite
{
    ShaderProgramDescriptorType shaderProgramDescriptorType;
    uint32_t bindingIndex;
    void* ptr;
}; 

struct ShaderProgramDescriptorSets
{
    PC_CORE_API virtual void WriteDescriptorSets(std::vector<ShaderProgramDescriptorWrite> shaderProgramDescriptorSet) = 0;
    
    PC_CORE_API ShaderProgramDescriptorSets() = default;

    PC_CORE_API virtual ~ShaderProgramDescriptorSets() = default;
private:
};

END_PCCORE