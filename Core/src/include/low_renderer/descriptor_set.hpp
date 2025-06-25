#pragma once

#include <vector>

#include "core_header.hpp"

#include "rendering/gpu_resource.hpp"

#include "rhi_sampler.hpp"
#include "rendering/sampler.hpp"

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
        IGpuResource* texture;
    };

    struct UniformBufferDescriptor
    {
        IGpuResource* buffer;
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
