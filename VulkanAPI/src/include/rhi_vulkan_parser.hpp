#pragma once

#include "vulkan_header.h"
#include "low_renderer/rhi_typedef.h"

namespace Vulkan
{
    vk::Format RHIFormatToVkFormat(PC_CORE::RHIFormat rhiFormat);
    
    vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType);
    
    vk::VertexInputRate RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate);
    
    vk::ImageType RHIImageToVkImageType(PC_CORE::ImageType _imageType);
    
    vk::ImageViewType RHIImageTypeToVulkanImageViewType(PC_CORE::ImageType _imageType);
    
    inline vk::ComponentSwizzle RHIToVulkanComponentSwizzle(PC_CORE::ComponentSwizzle _componentSwizzle)
    {
        return static_cast<vk::ComponentSwizzle>(_componentSwizzle);
    }

    vk::Filter RHIToVulkanFilter(PC_CORE::Filter _filter);

    vk::SamplerMipmapMode RHIToSamplerMipmapMode(PC_CORE::SamplerMipmapMode _mipmapMode);

    vk::SamplerAddressMode RHIToVulkanSamplerAddressMode(PC_CORE::SamplerAddressMode _samplerAddressMode);

    vk::CompareOp RHIToVulkanCompareOp(PC_CORE::CompareOp _compareOp);

    vk::BorderColor RHIToBorderColor(PC_CORE::BorderColor _borderColor);
        
    vk::PolygonMode RhiPolygonModeToVulkan(PC_CORE::PolygonMode _polygonMode);

    vk::ImageAspectFlags RhiTextureAspectMaskToVulkan(PC_CORE::TextureAspect _textureAspect);

    vk::ShaderStageFlagBits RhiToShaderStage(PC_CORE::ShaderStageType _shaderStage);

    vk::CullModeFlags RhiToCullMode(PC_CORE::CullModeFlagBit _cullModeFlagBit);

    vk::BufferUsageFlags RhiToBufferUsage(PC_CORE::BufferUsage _usage);
}

