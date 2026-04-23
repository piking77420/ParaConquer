#include "LowRenderer/RhiDescriptorSet.hpp"

#include "LowRenderer/Rhi.hpp"

#include <bit>

namespace PC_CORE
{

RhiDescriptorSet::RhiDescriptorSet(Rhi& _Rhi)
: RhiObjectT(_Rhi)
{

}

RhiDescriptorSet::~RhiDescriptorSet()
{

}

RhiDescriptorSet& RhiDescriptorSet::BindUniformBuffer(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiBuffer* _RhiBuffer)
{
	const BufferDescriptor Descriptor =
	{
		.buffer = _RhiBuffer,
	};

	DescriptorWrite Write
	{
		.type = _RhiBuffer->GetBufferBackingStrategy() == RhiBuffer::CpuVisibleRing ? DescriptorType::DynamicUniformBuffer : DescriptorType::UniformBuffer,
		.bindingIndex = _BindingIndex,
		.StagesBits = ShaderStageBits,
		.descriptor = Descriptor
	};

	m_Bindings.emplace_back(std::move(Write));
	return *this;
}

RhiDescriptorSet& RhiDescriptorSet::BindShaderStorageBuffer(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiBuffer* _RhiBuffer)
{
	const BufferDescriptor Descriptor =
	{
		.buffer = _RhiBuffer,
	};

	DescriptorWrite Write
	{
		.type = _RhiBuffer->GetBufferBackingStrategy() == RhiBuffer::CpuVisibleRing ? DescriptorType::DynamicStorageBuffer : DescriptorType::StorageBuffer,
		.bindingIndex = _BindingIndex,
		.StagesBits = ShaderStageBits,
		.descriptor = Descriptor
	};

	m_Bindings.emplace_back(std::move(Write));
	return *this;
}

RhiDescriptorSet& RhiDescriptorSet::BindTexture(RhiShaderStageTypeFlag _ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture, const RhiSampler* _RhiSampler)
{
	m_MaxBindingIndex = std::max(static_cast<size_t>(_BindingIndex), m_MaxBindingIndex);
	assert(_RhiSampler && _RhiTexture);

	const RhiResourceState resourceState = RhiResourceState::Undefined;

	const ImageSamplerDescriptor Descriptor =
	{
		.Texture = _RhiTexture,
		.Sampler = _RhiSampler,
		.ResourceState = _ShaderStageBits & RhiShaderStageBits::Compute ? RhiResourceState::ComputeRead : RhiResourceState::PixelShaderResource 
	};

	DescriptorWrite Write
	{
		.type = DescriptorType::CombinedImageSampler,
		.bindingIndex = _BindingIndex,
		.StagesBits = _ShaderStageBits,
		.descriptor = Descriptor
	};

	m_Bindings.emplace_back(std::move(Write));
	return *this;
}

RhiDescriptorSet& RhiDescriptorSet::BindImageLoad(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture)
{
	m_MaxBindingIndex = std::max(static_cast<size_t>(_BindingIndex), m_MaxBindingIndex);
	assert(_RhiTexture);
	const RhiResourceState resourceState = RhiResourceState::Undefined;

	assert(ShaderStageBits & RhiShaderStageBits::Compute);

	const ImageDescriptor Descriptor =
	{
		.Texture = _RhiTexture,
		.ResourceState = RhiResourceState::ComputeRead
	};

	DescriptorWrite Write
	{
		.type = DescriptorType::SampledImage,
		.bindingIndex = _BindingIndex,
		.StagesBits = ShaderStageBits,
		.descriptor = Descriptor
	};

	m_Bindings.emplace_back(std::move(Write));
	return *this;
}

RhiDescriptorSet& RhiDescriptorSet::BindImageWrite(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture)
{
	m_MaxBindingIndex = std::max(static_cast<size_t>(_BindingIndex), m_MaxBindingIndex);

	assert(_RhiTexture);
	const RhiResourceState resourceState = RhiResourceState::Undefined;

	assert(ShaderStageBits & RhiShaderStageBits::Compute);

	const ImageDescriptor Descriptor =
	{
		.Texture = _RhiTexture,
		.ResourceState = RhiResourceState::ComputeWrite
	};

	DescriptorWrite Write
	{
		.type = DescriptorType::StorageImage,
		.bindingIndex = _BindingIndex,
		.StagesBits = ShaderStageBits,
		.descriptor = Descriptor
	};

	m_Bindings.emplace_back(std::move(Write));
	return *this;
}

RhiDescriptorSet& RhiDescriptorSet::BindImageLoadWrite(RhiShaderStageTypeFlag ShaderStageBits, uint32_t _BindingIndex, const RhiTexture* _RhiTexture)
{
	m_MaxBindingIndex = std::max(static_cast<size_t>(_BindingIndex), m_MaxBindingIndex);
	assert(_RhiTexture);
	const RhiResourceState resourceState = RhiResourceState::Undefined;

	assert(ShaderStageBits & RhiShaderStageBits::Compute);

	const ImageDescriptor Descriptor =
	{
		.Texture = _RhiTexture,
		.ResourceState = RhiResourceState::ComputeReadWrite
	};

	DescriptorWrite Write
	{
		.type = DescriptorType::StorageImage,
		.bindingIndex = _BindingIndex,
		.StagesBits = ShaderStageBits,
		.descriptor = Descriptor
	};

	m_Bindings.emplace_back(std::move(Write));

	return *this;
}

}
