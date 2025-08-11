#pragma once

#include <Vector>

#include "CoreHeader.hpp"

#include "Rendering/GpuResource.hpp"

#include "RhiSampler.hpp"
#include "Rendering/Sampler.hpp"

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
        ImageState imageState;
    };

    struct ImageSamplerDescriptor
    {
        Sampler* sampler;
        IGpuResource* texture;
        ImageState imageState;
    };

    struct InputAttachementDescriptor
    {
        IGpuResource* image;
        ImageState imageState;
    };

    using Descriptor = std::variant<UniformBufferDescriptor, ImageSamplerDescriptor, InputAttachementDescriptor, ImageDescriptor>;
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
