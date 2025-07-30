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
	    CombinedImageSampler,
	    SampledImage,
	    StorageImage,
	    UniformBuffer,
	    StorageBuffer,
	    InputAttachment,
	    InlineUniformBlock,
	    AccelerationStructure,
	    Count,
    };

// TODO REFATOR with a variant

    struct UniformBufferDescriptor
    {
        IGpuResource* buffer;
    };

    struct ImageDescriptor
    {
        IGpuResource* texture;
    };

    struct ImageSamperDescriptor
    {
        Sampler* sampler;
        IGpuResource* texture;
    };
  
    struct InputAttachementDescriptor
    {
        IGpuResource* image;
    };

    using Descriptor = std::variant<UniformBufferDescriptor, ImageSamperDescriptor, ImageDescriptor, InputAttachementDescriptor>;
    struct ShaderProgramDescriptorWrite
    {
        ShaderProgramDescriptorType shaderProgramDescriptorType;
        uint32_t bindingIndex;

        Descriptor descriptor;
    };

    struct ShaderProgramDescriptorSets : public RhiResource
    {
        PC_CORE_API virtual void WriteDescriptorSets(
            const std::vector<PC_CORE::ShaderProgramDescriptorWrite>& shaderProgramDescriptorSet) = 0;

        PC_CORE_API ShaderProgramDescriptorSets() = default;

        PC_CORE_API virtual ~ShaderProgramDescriptorSets() = default;

    private:
    };

END_PCCORE
