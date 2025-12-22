#pragma once

#include "VulkanHeader.h"
#include "VulkanImageHelper.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "LowRenderer/DescriptorSet.hpp"
#include "LowRenderer/RhiShaderProgram.hpp"
#include "LowRenderer/RhiTexture.hpp"

namespace Vulkan::Utils
{
    vk::Format RhiFormatToVkFormat(PC_CORE::RhiFormat _rhiFormat);

    vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::RhiShaderProgram::PipelineType _shaderProgramPipelineType);

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

    vk::PolygonMode RhiPolygonModeToVulkan(PC_CORE::RhiShaderProgram::PolygonMode _polygonMode);

    vk::CullModeFlags RhiToCullMode(PC_CORE::RhiShaderProgram::CullModeFlag _cullModeFlagBit);

    vk::ShaderStageFlagBits RhiToShaderStage(PC_CORE::RhiShaderProgram::ShaderStageType _shaderStageType);

    vk::IndexType RhiToIndexType(PC_CORE::RhiBuffer::IndexFormat _format);

    vk::DescriptorType RhiToDescriptorType(const PC_CORE::ShaderProgramDescriptorType& shaderProgramDescriptorType);

    vk::SampleCountFlagBits RhSampleCountToVulkan(uint32_t _sampleCount);

    vk::PrimitiveTopology RhiPrimitiveTopology(PC_CORE::RhiShaderProgram::PrimitiveTopology _primitiveTopology);

    vk::AttachmentLoadOp RhiLoadOperationToVulkan(PC_CORE::LoadOperation _loadOperation);

    vk::AttachmentStoreOp RhiStoreOperationToVulkan(PC_CORE::StoreOperation _loadOperation);

    vk::PipelineStageFlags RhiPipelineStageToVulkan(PC_CORE::GpuPipelineStage _stageFlags);

    vk::BlendFactor RhiBlendFactorToVulkan(PC_CORE::BlendFactor _blendFactor);

    vk::BlendOp RhiBlendOpToVulkan(PC_CORE::BlendOp _blendOp);

    vk::ColorComponentFlags RhiColorComponent(PC_CORE::ColorComponent _colorComponent);

    vk::ImageUsageFlags GetImageUsageFlags(PC_CORE::RhiTexture::TextureUsageFlag _usage, vk::ImageAspectFlags aspectFlag);

    vk::ImageAspectFlags RhiTextureFormatToImageAspectFlagFlags(PC_CORE::RhiFormat _format);
    
    vk::ImageType RhiTextureTypeToVulkanImageType(PC_CORE::RhiTexture::Type _textureType);
    
    vk::BufferUsageFlags RhiBufferUsageToVulkan(PC_CORE::RhiBuffer::BufferUsageFlag _bufferUsage);

    vk::ImageLayout RhiResourceStateToVulkanImageLayout(RhiResourceState _rhiResourceState);

    RhiResourceState VulkanImageLayoutToResourceState(vk::ImageLayout layout);

    vk::AccessFlags RhiResourceStateToAccesFlag(RhiResourceState _RhiResourceState);
}
