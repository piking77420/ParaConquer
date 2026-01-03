#pragma once

#include <Vector>

#include "RhiResource.hpp"

BEGIN_PCCORE

class RhiTexture;
class RhiSampler;
class RhiBuffer;


enum class DescriptorType
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
    const RhiTexture* texture;
    RhiResourceState resourceState;
};

struct ImageSamplerDescriptor
{
    const RhiSampler* sampler;
    const RhiTexture* texture;
    RhiResourceState resourceState;
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
    Descriptor descriptor;
};

template<typename T>
concept DescriptorWriteType =
std::same_as<std::remove_cvref_t<T>, DescriptorWrite>;

class RhiDescriptorBindings : public RhiObjectT<RhiDescriptorBindings>
{
public:
    PC_CORE_API explicit RhiDescriptorBindings(Rhi& _Rhi);

    PC_CORE_API ~RhiDescriptorBindings() override;

    PC_CORE_API void SetBindings(std::initializer_list<DescriptorWrite> values)
    {
        m_Bindings = std::move(values);
    }

    PC_CORE_API void SetBindings(const std::initializer_list<DescriptorWrite>& values)
    {
        m_Bindings = values;
    }

    PC_CORE_API RhiDescriptorBindings& SetBindings(size_t _Set, const std::vector<DescriptorWrite>& values) noexcept
    {
        m_Bindings = values;
        m_Set = _Set;

        return *this;
    }

    PC_CORE_API RhiDescriptorBindings& SetBindings(size_t _Set, std::vector<DescriptorWrite>&& values) noexcept
    {
        m_Bindings = std::move(values);
        m_Set = _Set;

        return *this;
    }

    template <DescriptorWriteType ...T>
    RhiDescriptorBindings& SetBindings(size_t _Set, T&&... _Binding) noexcept
    {
        m_Bindings.clear();
        (m_Bindings.emplace_back(std::forward<T>(_Binding)), ...);
        m_Set = _Set;

        return *this;
    }

    template <DescriptorWriteType ...T>
    RhiDescriptorBindings& SetBindings(size_t _Set, const T&... _Binding) noexcept
    {
        m_Bindings.clear();
        (m_Bindings.push_back(_Binding), ...);
        m_Set = _Set;

        return *this;
    }

    size_t GetSet()
    {
        return m_Set;
    }

protected:
    const std::vector<DescriptorWrite>& GetBinding() const
    {
        return m_Bindings;
    }

private:
    std::vector<DescriptorWrite> m_Bindings;

    size_t m_Set = std::numeric_limits<size_t>::max();
};

END_PCCORE
