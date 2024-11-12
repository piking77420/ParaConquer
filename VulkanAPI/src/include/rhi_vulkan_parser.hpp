#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    vk::Format RHIFormatToVkFormat(PC_CORE::RHIFormat rhiFormat);
    
    vk::DescriptorType RHIDescriptorTypeToVulkan(PC_CORE::DescriptorType _descriptorType);

    vk::ShaderStageFlagBits RHIShaderStageToVulkan(const std::vector<PC_CORE::ShaderStageType>& _shaderStages);

    vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType);

    vk::BufferUsageFlags GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage);
    
    vk::VertexInputRate RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate);

    vk::CommandPoolCreateFlagBits CommandPoolCreateFlagBitsToVulkan(PC_CORE::CommandPoolBufferFlag _flag);
    
    uint32_t GetQueueFamiliesIndexFromType(VulkanContext* _context, PC_CORE::QueuType queueType);

    vk::ImageTiling RHiImageToVkImageTiling(PC_CORE::ImageTiling _imageTiling);

    vk::ImageType RHIImageToVkImageType(PC_CORE::ImageType _imageType);
    
    vk::ImageLayout RHIToVKImageLayout(PC_CORE::ImageLayout _imageLayout);

    vk::ImageViewType RHIImageTypeToVulkanImageViewType(PC_CORE::ImageType _imageType);
    

    inline vk::ImageViewCreateFlags RHIToVulkanImageViewCreateFlags(PC_CORE::ImageViewCreateInfoFlags _imageViewCreateInfoFlags)
    {
        return static_cast<vk::ImageViewCreateFlags>(_imageViewCreateInfoFlags);
    }

    inline vk::ComponentSwizzle RHIToVulkanComponentSwizzle(PC_CORE::ComponentSwizzle _componentSwizzle)
    {
        return static_cast<vk::ComponentSwizzle>(_componentSwizzle);
    }

    vk::SamplerCreateFlags RHIToVulkanSamplerCreateInfoFlags(PC_CORE::SamplerCreateInfoFlags _createInfoFlags);

    vk::Filter RHIToVulkanFilter(PC_CORE::Filter _filter);

    vk::SamplerMipmapMode RHIToSamplerMipmapMode(PC_CORE::SamplerMipmapMode _mipmapMode);

    vk::SamplerAddressMode RHIToVulkanSamplerAddressMode(PC_CORE::SamplerAddressMode _samplerAddressMode);

    vk::CompareOp RHIToVulkanCompareOp(PC_CORE::CompareOp _compareOp);

    vk::BorderColor RHIToBorderColor(PC_CORE::BorderColor _borderColor);
    
    vk::ImageViewType ImageTypeToImageViewType(vk::ImageType _imageType);
    
    vk::PolygonMode RhiPolygonModeToVulkan(PC_CORE::PolygonMode _polygonMode);

    vk::ImageAspectFlags RhiTextureAspectMaskToVulkan(PC_CORE::TextureAspect _textureAspect);
}

