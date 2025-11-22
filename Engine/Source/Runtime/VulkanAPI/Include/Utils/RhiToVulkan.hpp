#pragma once

#include "VulkanHeader.h"
#include "VulkanImageHelper.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "LowRenderer/DescriptorSet.hpp"
#include "LowRenderer/RhiShaderProgram.hpp"

namespace Vulkan::Utils
{
    vk::Format RhiFormatToVkFormat(PC_CORE::RhiFormat _rhiFormat);

    vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType);

    vk::VertexInputRate RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate);


    vk::ImageType RhiImageToVkImageType(PC_CORE::TextureType _textureType);

    vk::ImageViewType RhiImageToVkImageViewType(PC_CORE::TextureType _textureType);

    vk::ImageCreateFlags ImageCreateFlagFromTextureType(PC_CORE::TextureType _textureType);

    inline vk::ComponentSwizzle RhiToVulkanComponentSwizzle(PC_CORE::ComponentSwizzle _componentSwizzle)
    {
        return static_cast<vk::ComponentSwizzle>(_componentSwizzle);
    }

    vk::Filter RhiToVulkanFilter(PC_CORE::Filter _filter);

    vk::SamplerMipmapMode RhiToSamplerMipmapMode(PC_CORE::SamplerMipmapMode _mipmapMode);

    vk::SamplerAddressMode RhiToVulkanSamplerAddressMode(PC_CORE::SamplerAddressMode _samplerAddressMode);

    vk::CompareOp RhiToVulkanCompareOp(PC_CORE::CompareOp _compareOp);

    vk::BorderColor RhiToBorderColor(PC_CORE::BorderColor _borderColor);

    vk::PolygonMode RhiPolygonModeToVulkan(PC_CORE::PolygonMode _polygonMode);

    vk::CullModeFlags RhiToCullMode(PC_CORE::CullModeFlagBit _cullModeFlagBit);

    vk::ShaderStageFlagBits RhiToShaderStage(PC_CORE::ShaderStageType _shaderStageType);

    vk::IndexType RhiToIndexType(PC_CORE::IndexFormat _format);

    vk::DescriptorType RhiToDescriptorType(const PC_CORE::ShaderProgramDescriptorType& shaderProgramDescriptorType);

    vk::SampleCountFlagBits RhiSampleCountToVuklan(uint32_t _sampleCount);

    vk::PrimitiveTopology RhiPrimitiveTopology(PC_CORE::PrimitiveTopology _primitiveTopology);

    vk::AttachmentLoadOp RhiLoadOperationToVulkan(PC_CORE::LoadOperation _loadOperation);

    vk::AttachmentStoreOp RhiStoreOperationToVulkan(PC_CORE::StoreOperation _loadOperation);

    vk::PipelineStageFlags RhiPipelineStageToVulkan(PC_CORE::GpuPipelineStageFlagBits _stageFlags);

    vk::AccessFlags RhiAccessFlagToVulkan(PC_CORE::GpuAccessFlag _accessFlag);

    VmaMemoryUsage RhiMemoryUsageToVulkan(PC_CORE::MemoryLocalisation _memoryUsage);

    vk::BlendFactor RhiBlendFactorToVulkan(PC_CORE::BlendFactor _blendFactor);

    vk::BlendOp RhiBlendOpToVulkan(PC_CORE::BlendOp _blendOp);

    vk::ColorComponentFlags RhiColorComponent(PC_CORE::ColorComponent _colorComponent);

    vk::ImageLayout RhiImageStateToVulkanImageLayout(PC_CORE::ImageState _imageState);

    vk::ImageUsageFlags GetImageUsageFlags(PC_CORE::TextureUsage _textureUsage);

    vk::ImageAspectFlags RhiTextureUsageToImageAspectFlagFlags(PC_CORE::TextureUsage _textureUsage);
}
