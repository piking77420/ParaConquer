#pragma once

#include <Vector>

#include "RhiShaderProgram.hpp"

BEGIN_PCCORE

class RhiTexture;
class RhiSampler;
class RhiBuffer;


enum class DescriptorType : uint8_t
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

struct BufferDescriptor
{
    const RhiBuffer* buffer;
};

struct ImageDescriptor
{
    const RhiTexture* Texture;
    RhiResourceState ResourceState;
};

struct ImageSamplerDescriptor
{
    const RhiTexture* Texture;
    const RhiSampler* Sampler;
    RhiResourceState ResourceState;
};

struct InputAttachementDescriptor
{
    const RhiTexture* image;
    RhiResourceState resourceState;
};

using Descriptor = std::variant<std::monostate, BufferDescriptor, ImageSamplerDescriptor, InputAttachementDescriptor,
    ImageDescriptor>;

struct DescriptorWrite
{
    DescriptorType type;
    uint32_t bindingIndex;
    RhiShaderStageTypeFlag StagesBits;
    Descriptor descriptor;
};

// TODO HANDLE NON PER FRAME SET
class RhiDescriptorSet : public RhiObjectT<RhiDescriptorSet>
{
public:
 
    PC_CORE_API explicit RhiDescriptorSet(Rhi& _Rhi);

    PC_CORE_API ~RhiDescriptorSet() override;

    PC_CORE_API RhiDescriptorSet& BindUniformBuffer(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiBuffer* _RhiBuffer);

    PC_CORE_API RhiDescriptorSet& BindTexture(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture, const RhiSampler* _RhiSampler);

    PC_CORE_API RhiDescriptorSet& BindImageLoad(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture);

    PC_CORE_API RhiDescriptorSet& BindImageWrite(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture);

    PC_CORE_API RhiDescriptorSet& BindImageLoadWrite(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture);

protected:
    const std::vector<DescriptorWrite>& GetBinding() const
    {
        return m_Bindings;
    }

private:
    std::vector<DescriptorWrite> m_Bindings;
};

END_PCCORE
