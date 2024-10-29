#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    vk::Format RHIFormatToVkFormat(PC_CORE::RHIFormat rhiFormat);
    
    vk::DescriptorType RHIDescriptorTypeToVulkan(PC_CORE::DESCRIPTOR_TYPE _descriptorType);

    vk::ShaderStageFlagBits RHIShaderStageToVulkan(const std::vector<PC_CORE::ShaderStageType>& _shaderStages);

    vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType);

    vk::BufferUsageFlags GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage);
    
    vk::VertexInputRate RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate);

    vk::CommandPoolCreateFlagBits CommandPoolCreateFlagBitsToVulkan(PC_CORE::CommandPoolBufferFlag _flag);
    
    uint32_t GetQueueFamiliesIndexFromType(VulkanContext* _context, PC_CORE::QueuType queueType);

    vk::ImageTiling RHiImageToVkImageTiling(PC_CORE::ImageTiling _imageTiling);

    vk::ImageType RHIImageToVkImageType(PC_CORE::ImageType _imageType);

    vk::ImageAspectFlagBits RhiToVKImageAspectFlagBits(PC_CORE::ImageAspectFlagBits _imageAspectFlagBits);

    vk::ImageLayout RHIToVKImageLayout(PC_CORE::VkImageLayout _imageLayout);
}

