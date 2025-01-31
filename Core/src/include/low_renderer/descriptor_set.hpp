#pragma once

#include <vector>

#include "core_header.hpp"

BEGIN_PCCORE
    enum class ShaderProgramDescriptorType
    {
        Image,
        Sampler,
        UniformBuffer,
        eStorageBuffer,
        InlineUniformBlock,
        AccelerationStructure,
        Count,
    };

    struct ShaderProgramDescriptorWrite
    {
        ShaderProgramDescriptorType shaderProgramDescriptorType;
        uint32_t bindingIndex;
        void* object;
    };

    struct ShaderProgramDescriptorSets
    {
        PC_CORE_API virtual void WriteDescriptorSets(
            const std::vector<PC_CORE::ShaderProgramDescriptorWrite>& shaderProgramDescriptorSet) = 0;

        PC_CORE_API ShaderProgramDescriptorSets() = default;

        PC_CORE_API virtual ~ShaderProgramDescriptorSets() = default;

    private:
    };

END_PCCORE
