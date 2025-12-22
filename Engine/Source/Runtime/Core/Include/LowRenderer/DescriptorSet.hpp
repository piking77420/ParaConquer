#pragma once

#include <Vector>

#include "RhiResource.hpp"

BEGIN_PCCORE

class RhiTexture;
class RhiSampler;
class RhiBuffer;

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

struct BufferDescriptor
{
    RhiBuffer* buffer;
};

struct ImageDescriptor
{
    RhiTexture* texture;
    RhiResourceState resourceState;
};

struct ImageSamplerDescriptor
{
    RhiSampler* sampler;
    RhiTexture* texture;
    RhiResourceState resourceState;
};

struct InputAttachementDescriptor
{
    RhiTexture* image;
    RhiResourceState resourceState;
};

using Descriptor = std::variant<BufferDescriptor, ImageSamplerDescriptor, InputAttachementDescriptor,
                                ImageDescriptor>;

struct ShaderProgramDescriptorWrite
{
    ShaderProgramDescriptorType type;
    uint32_t bindingIndex;
    Descriptor descriptor;
};


class ShaderProgramDescriptorSets : public RhiObjectT<ShaderProgramDescriptorSets>
{
public:
    PC_CORE_API explicit ShaderProgramDescriptorSets(Rhi& _Rhi, const std::string& _name);

    PC_CORE_API explicit ShaderProgramDescriptorSets(Rhi& _Rhi, std::string&& _name);

    PC_CORE_API ~ShaderProgramDescriptorSets() override;

    PC_CORE_API void SetBindings(std::initializer_list<ShaderProgramDescriptorWrite> values)
    {
        m_Bindings = std::move(values);
    }

    PC_CORE_API void SetBindings(const std::initializer_list<ShaderProgramDescriptorWrite>& values)
    {
        m_Bindings = values;
    }

    PC_CORE_API ShaderProgramDescriptorSets& SetBindings(const std::vector<ShaderProgramDescriptorWrite>& values , size_t _Set)
    {
        m_Bindings = values;
        m_Set = _Set;

        return *this;
    }

    size_t GetSet()
    {
        return m_Set;
    }

protected:
    const std::vector<ShaderProgramDescriptorWrite>& GetBinding() const
    {
        return m_Bindings;
    }

private:
    std::vector<ShaderProgramDescriptorWrite> m_Bindings;

    size_t m_Set = std::numeric_limits<size_t>::max();
};

END_PCCORE
