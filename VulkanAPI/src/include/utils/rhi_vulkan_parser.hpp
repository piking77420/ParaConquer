#pragma once

#include "vulkan_header.h"
#include "vulkan_image_helper.hpp"
#include "low_renderer/rhi_typedef.h"
#include "low_renderer/descriptor_set.hpp"
#include "low_renderer/rhi_shader_program.hpp"

namespace Vulkan
{
    namespace Utils
    {
        
        vk::Format RHIFormatToVkFormat(PC_CORE::RHIFormat rhiFormat);
    
        vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType);
    
        vk::VertexInputRate RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate);

        
        vk::ImageType RHIImageToVkImageType(PC_CORE::TextureType _textureType);
        
        vk::ImageViewType RHIImageToVkImageViewType(PC_CORE::TextureType _textureType);

        vk::ImageCreateFlags ImageCreateFlagFromTextureType(PC_CORE::TextureType _textureType);

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
    
        vk::ShaderStageFlagBits RhiToShaderStage(PC_CORE::ShaderStageTypeFlag _shaderStage);

        vk::CullModeFlags RhiToCullMode(PC_CORE::CullModeFlagBit _cullModeFlagBit);
        
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
        
    }
    
}

