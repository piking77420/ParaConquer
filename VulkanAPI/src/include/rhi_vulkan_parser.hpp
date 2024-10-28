#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    vk::DescriptorType RHIDescriptorTypeToVulkan(PC_CORE::DESCRIPTOR_TYPE _descriptorType);

    vk::ShaderStageFlagBits RHIShaderStageToVulkan(const std::vector<PC_CORE::ShaderStageType>& _shaderStages);

    vk::PipelineBindPoint RhiPipelineBindPointToVulkan(PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType);

    vk::BufferUsageFlags GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage);
}

