#pragma once

#include <VulkanHeader.h>
#include <LowRenderer/RhiTypedef.h>
#include <LowRenderer/RhiDescriptorSet.hpp>
#include <LowRenderer/RhiPipeline.hpp>
#include <LowRenderer/RhiTexture.hpp>
#include <LowRenderer/RhiBuffer.h>

namespace Vulkan::Utils
{
    vk::Format RhiFormatToVkFormat(PC_CORE::RhiFormat _rhiFormat);

    vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::RhiPipeline::PipelineType _shaderProgramPipelineType);

    vk::VertexInputRate RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate);
    
    vk::ImageType RhiImageToVkImageType(PC_CORE::RhiTexture::Type _textureType);

    vk::ImageViewType RhiImageToVkImageViewType(PC_CORE::RhiTexture::Type _textureType);

    vk::ImageCreateFlags ImageCreateFlagFromTextureType(PC_CORE::RhiTexture::Type _textureType);

    inline vk::ComponentSwizzle RhiToVulkanComponentSwizzle(PC_CORE::ComponentSwizzle _componentSwizzle)
    {
        return static_cast<vk::ComponentSwizzle>(_componentSwizzle);
    }

    vk::Filter RhiToVulkanFilter(PC_CORE::Filter _filter);

    vk::SamplerMipmapMode RhiToSamplerMipmapMode(PC_CORE::SamplerMipmapMode _mipmapMode);

    vk::SamplerAddressMode RhiToVulkanSamplerAddressMode(PC_CORE::SamplerAddressMode _samplerAddressMode);

    vk::CompareOp RhiToVulkanCompareOp(PC_CORE::CompareOp _compareOp);

    vk::BorderColor RhiToBorderColor(PC_CORE::BorderColor _borderColor);

    vk::PolygonMode RhiPolygonModeToVulkan(PC_CORE::RhiPipeline::PolygonMode _polygonMode);

    vk::CullModeFlags RhiToCullMode(PC_CORE::RhiPipeline::CullModeFlag _cullModeFlagBit);

    vk::ShaderStageFlags RhiToShaderStage(RhiShaderStageTypeFlag _RhiShaderStageTypeFlag);

    vk::ShaderStageFlagBits RhiToShaderStageBits(RhiShaderStageBits _RhiShaderStageBits);

    vk::IndexType RhiToIndexType(PC_CORE::RhiBuffer::IndexFormat _format);

    vk::DescriptorType RhiToDescriptorType(const PC_CORE::DescriptorType& descriptorType);

    vk::SampleCountFlagBits RhSampleCountToVulkan(uint32_t _sampleCount);

    vk::PrimitiveTopology RhiPrimitiveTopology(PC_CORE::RhiPipeline::PrimitiveTopology _primitiveTopology);

    vk::AttachmentLoadOp RhiLoadOperationToVulkan(PC_CORE::LoadOperation _loadOperation);

    vk::AttachmentStoreOp RhiStoreOperationToVulkan(PC_CORE::StoreOperation _loadOperation);

    vk::PipelineStageFlags RhiPipelineStageToVulkan(PC_CORE::GpuPipelineStage _stageFlags);

    vk::BlendFactor RhiBlendFactorToVulkan(PC_CORE::BlendFactor _blendFactor);

    vk::BlendOp RhiBlendOpToVulkan(PC_CORE::BlendOp _blendOp);

    vk::ColorComponentFlags RhiColorComponent(PC_CORE::ColorComponent _colorComponent);

    vk::ImageUsageFlags GetImageUsageFlags(PC_CORE::RhiTexture::TextureUsageFlag _usage);

    vk::ImageAspectFlags RhiTextureFormatToImageAspectFlagFlags(PC_CORE::RhiFormat _format);
    
    vk::ImageType RhiTextureTypeToVulkanImageType(PC_CORE::RhiTexture::Type _textureType);
    
    vk::BufferUsageFlags RhiBufferUsageToVulkan(PC_CORE::RhiBuffer::BufferUsageFlag _bufferUsage);

    vk::ImageLayout RhiResourceStateToVulkanImageLayout(RhiResourceState _rhiResourceState);

    vk::AccessFlags RhiResourceStateToAccesFlag(RhiResourceState _RhiResourceState);

    vk::PipelineStageFlags PipelineStageFlagsFromRhiResourceState(RhiResourceState _RhiResourceState);
}
