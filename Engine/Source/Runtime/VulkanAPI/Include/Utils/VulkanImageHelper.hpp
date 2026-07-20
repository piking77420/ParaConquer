#pragma once

#include <vk_mem_alloc.h>

#include "VulkanHeader.h"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiTexture.hpp"

namespace Vulkan::Utils
{

    VULKAN_API void GenerateMipMapFunc(
        vk::CommandBuffer _CommandBuffer,
        vk::Image _Image,
        vk::Filter _Filter,
        vk::AccessFlags _OldAccesFlag,
        vk::ImageLayout _OldImageLayout,
        vk::PipelineStageFlags _OldPipelineStageFlags,
        vk::AccessFlags _NewAccesFlag,
        vk::ImageLayout _NewImageLayout,
        vk::PipelineStageFlags _NewPipelineStageFlags,
        int32_t _ImageWidth,
        int32_t _ImageHeight,
        vk::Format _Format,
        uint32_t _MipLevel,
        uint32_t _LayerCount,
        vk::ImageAspectFlags _AspectFlag);

}
