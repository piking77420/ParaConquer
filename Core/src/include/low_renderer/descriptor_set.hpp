#pragma once

#include <vector>

#include "core_header.hpp"
#include "gpu_buffer.hpp"
#include "gpu_handle.hpp"
#include "sampler.hpp"

BEGIN_PCCORE
    enum class ShaderProgramDescriptorType
    {
        Sampler,
        CombineImageSampler,
        SampledImage,
        UniformBuffer,
        StorageBuffer,
        InlineUniformBlock,
        AccelerationStructure,
        Count,
    };

    struct ImageSamperDescriptor
    {
        Sampler* sampler;
        GpuHandle* imageHandle;
    };

    struct UniformBufferDescriptor
    {
        GpuBuffer* buffer;
    };


    struct ShaderProgramDescriptorWrite
    {
        ShaderProgramDescriptorType shaderProgramDescriptorType;
        uint32_t bindingIndex;
        UniformBufferDescriptor* uniformBufferDescriptor;
        ImageSamperDescriptor* imageSamperDescriptor;
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
